#include <iostream>
#include <stdlib.h>

using namespace std;

class assembled_chunk {
	long int t0;
	unsigned char *data;
	
	assembled_chunk(long int t0;);

	~assembled_chunk();

	set_data(int i, int x);

};


class vdif_assembler {
	int port, number_of_processors;
	int start_index, end_index;

	unsigned char *data_buf, *header_buf;
	vdif_processors processors;

	vdif_assembler(int port);

	~vdif_assembler();

	int register_processor(const std::shared_ptr<vdif_processor> &p);
	void run();
	int assemble_chunk(unsigned char *data_buf, int *start_index, int *end_index, int *chunk_start);

	void start_async();
	void wait_until_end();
};

struct header {
	long int t0;
	int polarization;
};