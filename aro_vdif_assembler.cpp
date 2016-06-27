#include <sys/socket.h>
#include <fstream>
#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "aro_vdif_assembler.hpp"

namespace aro_vdif_assembler
{

std::mutex mtx;
std::condition_variable cv;

assembled_chunk::assembled_chunk(long int start_time) {
	
	data = new unsigned char[constants::chunk_size * constants::nfreq];
	t0 = start_time;
	nt = constants::chunk_size/2;
}

assembled_chunk::~assembled_chunk() {
	delete[] data;
}

void assembled_chunk::set_data(int i, unsigned char x) {
	data[i] = x;
}

vdif_assembler::vdif_assembler(const char *arg1, const char *arg2){
	
	if (strcmp("network",arg1)==0) {
		temp_buf = new unsigned char[constants::udp_packets * 1056];
		mode = 1;
		port = atoi(arg2);
	} else if (strcmp("disk",arg1)==0) {
		temp_buf = new unsigned char[constants::file_packets * 1056];
		mode = 0;
		filelist_name = new char[strlen(arg2)];
		strcpy(filelist_name,arg2);
	} else {
		std::cout << "Unsupported option." << std::endl;
		exit(1);
	}

	processors = new vdif_processor *[constants::max_processors];
	number_of_processors = 0;
	data_buf = new unsigned char[constants::buffer_size * constants::nfreq];
	header_buf = new struct header[constants::buffer_size];
	bufsize = 0;
	start_index = 0;
	end_index = 0;
	processor_threads = new std::thread[constants::max_processors];
	

}

vdif_assembler::~vdif_assembler() {
	delete[] processors;
	delete[] data_buf;
	delete[] header_buf;
}

int vdif_assembler::register_processor(vdif_processor *p) {

	if (number_of_processors < constants::max_processors) {

		processors[number_of_processors] = p;
		number_of_processors++;
		return 1;
	} else {
		std::cout << "The assembler is full, can't register any new processors." << std::endl;
		return 0;
	}
}

int vdif_assembler::kill_processor(vdif_processor *p) {
	for (int i = 0; i < number_of_processors; i++) {
		if (processors[i] == p) {
			for (int j = i; j < (number_of_processors - 1); j++) {
				processors[j] = processors[j + 1];
			}
			number_of_processors--;
			return 1;
		}
	}
	std::cout << "Unable to find this processor.";
	return 0;
}

int vdif_assembler::is_full() {

	return bufsize == constants::buffer_size;
}


void vdif_assembler::run() {

	std::thread assemble_t(&vdif_assembler::assemble_chunk,this);
	std::thread stream_t;

	if (mode) {
		stream_t = std::thread(&vdif_assembler::network_capture,this);
	} else {
		stream_t = std::thread(&vdif_assembler::read_from_disk,this);
	}
	
	stream_t.join();
	assemble_t.join();

}


void vdif_assembler::assemble_chunk() {
	
	for (;;) {
		//cout << " start: " << start_index << " end: " << end_index << endl;
		std::unique_lock<std::mutex> lk(mtx);

		if (bufsize < constants::chunk_size) {
			cv.wait(lk);
		}

		std::cout << "Chunk found" << std::endl;
		assembled_chunk c(header_buf[end_index].t0);
		
		for (int i = 0; i < constants::chunk_size * constants::nfreq; i++) {
			c.set_data(i, data_buf[start_index+i]);
		}
		if (chunks.size() < constants::max_chunks) {
			chunks.push(&c);
		}
		start_index = (start_index + constants::chunk_size) % constants::buffer_size;

		bufsize -= constants::chunk_size;
	
		std::cout << "excess: " << bufsize << std::endl;

		lk.unlock();
		
	}
}


void vdif_assembler::network_capture() {
	
	int size = constants::udp_packets * 1056;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd < 0) {
		std::cout << "socket failed." << std::endl;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(sock_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		std::cout << "bind failed." << std::endl;
	}

	for (;;) {
		if (read(sock_fd, temp_buf, size) == size) {
			std::unique_lock<std::mutex> lk(mtx);
			if (!is_full()) {
				vdif_read(temp_buf, size);
			}
			else {
				std::cout << "Buffer is full. Dropping packets." << std::endl;
			}
			lk.unlock();		
		}
	}

}
void vdif_assembler::read_from_disk() {        
	
	std::ifstream fl(filelist_name, std::ifstream::in);
        std::string filename;
        
        int bytes_read;	
	if (!fl) {
		std::cout << "Cannot open filelist." << std::endl;
		exit(1);
	}
        while (getline(fl, filename)){
		bytes_read = 0;
		FILE *fp = fopen(filename.c_str(), "r");
                if (!fp) {
                       std::cout << "can't open " << filename << std::endl;
                      	continue; 
                }
		std::cout << "Reading " << filename << std::endl;
		
               	while ((bytes_read < constants::file_packets * 1056) && !(feof(fp))) {
			fread(&temp_buf[bytes_read],sizeof(temp_buf[bytes_read]),1,fp);
			bytes_read++;
		}
		
		vdif_read(temp_buf,bytes_read);
                
		fclose(fp);
		
        }
}

assembled_chunk* vdif_assembler::get_chunk() {
	
	assembled_chunk* temp;
	if (chunks.size()) {
		temp = chunks.front();
		chunks.pop();
		return temp;
	} else {
		std::cout << "no chunk available at this moment." << std::endl;
		return 0;
	}

}


void vdif_assembler::vdif_read(unsigned char *data, int size) {


	int word[8];
	int count = 0;
	long int t0 = 0;
	int pol = 0;
	int nmissing = 0;
	long int current,expect = 0;
	bool invalid;
	
	while ((count < size) && (!is_full())) {
		
		for (int i = 0; i < 8; i++) {
			word[i] = (data[count + 3] << 24) + (data[count + 2] << 16) + (data[count + 1] << 8) + data[count];
			count += 4;
		}
		
		invalid = (word[0] >> 31);
		
 		if (invalid) {
			fill_missing(1);
		}
		
		t0 = (long int) (word[0] & 0x3FFFFFFF) * (long int) constants::frame_per_second + (long int) (word[1] & 0xFFFFFF);
		
		pol = (word[3] >> 16) & 0x3FF;
		
		current = t0 * 2 + pol;

		if (expect) {
			nmissing = (int) (expect - current);
		}

		if (nmissing) {
			std::cout << "current: " << current << " expect: " << expect << std::endl; 
			std::cout << "start: " << start_index << " end: " << end_index << std::endl;
			fill_missing(nmissing);
		}

		expect = current + 1;
		header_buf[end_index].t0 = t0;
		header_buf[end_index].polarization = pol;
		
		for (int i = 0; i < constants::nfreq; i++) {
			data_buf[end_index * constants::nfreq + i] = data[count];
			count++;
		}
		end_index = (end_index + 1) % constants::buffer_size;
		bufsize++;
		
		if (bufsize >= constants::chunk_size) {
			cv.notify_one();
		} 
	
		

		//cout << "start: " << start_index << " end: " << end_index << " size: " << bufsize << endl;
	}
	
}

void vdif_assembler::start_async(){
	std::thread run_t(&vdif_assembler::run, this);
	run_t.detach();
}

//TODO write
void vdif_assembler::wait_until_end(){}


vdif_processor::vdif_processor(const std::string &name_, bool is_critical_=false){
	name = name_;
	is_critical = is_critical_;
	runflag = false;
	//pthread_mutex_init(&mutex, NULL);
}

vdif_processor::~vdif_processor(){

}

bool vdif_processor::is_running(){
	// pthread_mutex_lock(&mutex);
	// bool ret = runflag;
	// pthread_mutex_unlock(&mutex);

	return true;
}

void vdif_assembler::fill_missing(int n) {
	long int prev_t0 = header_buf[(end_index-1) % constants::buffer_size].t0;
	int prev_pol = header_buf[(end_index-1) % constants::buffer_size].polarization;
	//cout << "Missing " << n << " packets." << endl;
	for (int i = 0; i < n; i++) {
		prev_t0++;
		prev_pol = (prev_pol + 1) % 2;
		header_buf[end_index].t0 = prev_t0;
		header_buf[end_index].polarization = prev_pol;
		for (int j = 0; j < constants::nfreq; j++) {
			data_buf[end_index * constants::nfreq + j] = 0;
		}
		end_index = (end_index + 1) % constants::buffer_size;
		bufsize++;
	}

}


// base_python_processor::base_python_processor(char* name): vdif_processor(name){}

// void base_python_processor::process_chunk(const assembled_chunk* a){
// 	ref_chunk = a;
// }

// void base_python_processor::initialize(){}

// void base_python_processor::finalize(){}

// base_python_processor::~base_python_processor(){}

// base_python_processor* make_python_processor(void (*callback_)(assembled_chunk*)){
// 	return new base_python_processor("python_processor",callback_);
// }

}

