#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "vdif_assembler.hpp"

namespace vdif_assembler{

assembled_chunk *c = new assembled_chunk(0,constants::num_time);
std::mutex mtx;
std::condition_variable cv;

assembled_chunk::assembled_chunk(long int start_time, int my_nt) {
	
	data = new unsigned char[constants::chunk_size * constants::nfreq];
	t0 = start_time;
	nt = my_nt;

}

assembled_chunk::~assembled_chunk() {
	delete[] data;
}

void assembled_chunk::set_data(int i, unsigned char x) {
	data[i] = x;
}

// void vdif_processor::process_chunk(assembled_chunk *c) {
// 	std::cout << c->t0 << std::endl;
// 	this_thread::sleep_for(chrono::milliseconds(int(constants::chunk_size/2*2.56/1000)));
// 	cout << "Processing chunk done." << endl;

// }


vdif_assembler::vdif_assembler(short unsigned int my_port){
	port = my_port;
	processor_threads = new std::thread [constants::max_processors];
	processors = new vdif_processor *[constants::max_processors];
	number_of_processors = 0;
	data_buf = new unsigned char[constants::buffer_size * constants::nfreq];
	header_buf = new struct header[constants::buffer_size];
	bufsize = 0;
	start_index = 0;
	end_index = 0;

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
	if (bufsize == constants::buffer_size) {
		return 1;
	}
	else {
		return 0;
	}
}


void vdif_assembler::run() {

	std::thread assemble_t(&vdif_assembler::assemble_chunk, this);
	std::thread net_t(&vdif_assembler::network_capture,this);
	net_t.join();
	assemble_t.join();

}


void vdif_assembler::assemble_chunk() {
	//assume no missing packets, everything in order


	for (;;) {
		//cout << " start: " << start_index << " end: " << end_index << endl;
		std::unique_lock<std::mutex> lk(mtx);

		if (bufsize < constants::chunk_size) {
			cv.wait(lk);
		}
			
		std::cout << "Chunk found" << std::endl;
		//cout << "start: " << start_index << " end: " << end_index << endl;
		c->t0 = header_buf[start_index].t0;
		
		for (int i = 0; i < constants::chunk_size * constants::nfreq; i++) {
			c->set_data(i, data_buf[start_index*constants::nfreq+i]);
		}
		start_index += constants::chunk_size;

		bufsize -= constants::chunk_size;
		std::cout << "excess: " << bufsize << std::endl;
		lk.unlock();

		if (start_index >= constants::buffer_size) {
			start_index -= constants::buffer_size;
		}

		for (int i = 0; i < number_of_processors; i++) {
			processor_threads[i]=std::thread(&vdif_processor::process_chunk,processors[i],c);
		}
		for (int i = 0; i < number_of_processors; i++) {
			processor_threads[i].join();
		}
			
		
	}
}


void vdif_assembler::network_capture() {

	int size = 1056 * 32;

	unsigned char dgram[size];

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
		if (read(sock_fd, dgram, sizeof(dgram)) == size) {
			std::unique_lock<std::mutex> lk(mtx);
			if (!is_full()) {
				vdif_read(dgram, size);
			}
			else {
				std::cout << "Buffer is full. Dropping packets." << std::endl;
			}
			lk.unlock();		
		}
	}

}

void vdif_assembler::vdif_read(unsigned char *data, int size) {


	int word[8];
	int count = 0;

	while ((count < size) && (!is_full())) {
		for (int i = 0; i < 8; i++) {
			word[i] = (data[count + 3] << 24) + (data[count + 2] << 16) + (data[count + 1] << 8) + data[count];
			count += 4;
		}
		header_buf[end_index].t0 =(long int)(word[0] & 0x3FFFFFFF) * (long int)(constants::frame_per_second) + (long int) (word[1] & 0xFFFFFF);
		header_buf[end_index].polarization = (word[3] >> 16) & 0x3FF;
		//cout << "time: " << (word[0] & 0x3FFFFFFF) << " frame: " << (word[1] & 0xFFFFFF) << endl;
		//cout << header_buf[end_index].t0 << endl;
		for (int i = 0; i < constants::nfreq; i++) {
			data_buf[end_index*constants::nfreq+i] = data[count];
			count++;
		}
		end_index++;
		
		bufsize++;
		
		if (bufsize >= constants::chunk_size) {
			cv.notify_one();

		} 
	
		
		if (end_index >= constants::buffer_size) {
			end_index -= constants::buffer_size;
		}
		

		//cout << "start: " << start_index << " end: " << end_index << " size: " << bufsize << endl;
	}
	
}

void vdif_assembler::start_async(){
	std::thread run_t(&vdif_assembler::run, this);
	run_t.detach();
}

//TODO expand
void vdif_assembler::wait_until_end(){}


vdif_processor::vdif_processor(const std::string &name_, bool is_critical_=false){
	name = name_;
	is_critical = is_critical_;
	runflag = false;
	pthread_mutex_init(&mutex, NULL);
}

vdif_processor::~vdif_processor(){

}

bool vdif_processor::is_running(){
	pthread_mutex_lock(&mutex);
	bool ret = runflag;
	pthread_mutex_unlock(&mutex);

	return ret;
}
void vdif_processor::set_running(){
	pthread_mutex_lock(&mutex);

	if (runflag) {
	pthread_mutex_unlock(&mutex);

	//FIX
	//throw_rerun_exception();
	}

	runflag = true;
	pthread_mutex_unlock(&mutex);
}
}
