#include "vdif_assembler.hpp"
#include "network_capture.cpp"


namespace aro_vdif_assembler{
#if 0
}; // pacify emacs c-mode
#endif

namespace constants{

	const int nfreq = 1024;
	const int header_size = 12; //int64 time + int32 thread ID
	const int chunk_size = 65536;
	const int max_processors = 10;
	const int frame_per_second = 390625;
	const int buffer_size = 524288; //at most 8 chunk in buffer

}

assembled_chunk::assembled_chunk(long int start_time) {
	
	data = new unsigned char[constants::chunk_size];
	t0 = start_time;

}

assembled_chunk::~assembled_chunk() {
	delete[] data;
}

assembled_chunk::set_data(int i, int x) {
	data[i] = x;
}

 


struct vdif_processor {};

vdif_assembler::vdif_assembler(int port_number){
	port = port_number;
	vdif_processors processors[constants::max_processors];
	number_of_processors = 0;
	data_buf = new unsigned char[constants::buffer_size * constants:nfreq];
	header_buf = new struct header[constants::buffer_size];
	start_index = 0;
	end_index = 0;

}

vdif_assembler::~vdif_assembler() {
	delete[] vdif_processors;
}

int vdif_assembler::register_processor(vdif_processor *p) {
	if (number_of_processors < cosntant::max_processors) {
		processors[number_of_processors] = p;
		number_of_processors++;
		return 1;
	} else {
		cout << "The assembler is full, can't register any new processors." << endl;
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
	cout << "Unable to find this processor."
	return 0;
}

void vdif_assembler::run() {
	
	network_capture(port, header_buf, data_buf, &start_index, &end_index);
	int *chunk_start;
	long int timestamp = assemble_chunk(data_buf, &start_index, &end_index, &chunk_start);
	if (timestamp) {
		assembled_chunk *c = new assembled_chunk(timestamp);
		for (int i = 0; i < constants::chunk_size; i++) {
			c.set_data(i, chunk_start);
			(*chunk_start)++;
		}
		for (int i = 0; i < number_of_processors; i++) {
			processors[i].process_chunk(c);
		}
	}

}

}