#pragma once
#include <iostream>
#include <stdlib.h>
//#include "vdif_processor.hpp"

#ifndef _unlikely
#define _unlikely(cond)  (__builtin_expect(cond,0))
#endif

#ifndef xassert
#define xassert(cond) xassert2(cond, __LINE__)
#define xassert2(cond,line) \
	do { \
		if (_unlikely(not cond)) { \
		const char *msg = "Assertion '" __STRING(cond) "' failed (" __FILE__ ":" __STRING(line) ")\n"; \
		std::cout << msg << std::flush; \
		throw std::runtime_error(msg); \
	} \
	} while (0)
#endif



namespace vdif_assembler{

namespace constants{

	const int nfreq = 1024;
	const int header_size = 12; //int64 time + int32 thread ID
	const int chunk_size = 65536;
	const int num_time = 1024;
	const int max_processors = 10;
	const int frame_per_second = 390625;
	const int buffer_size = 524288; //at most 8 chunk in buffer

};

struct header {
	long unsigned int t0;
	int polarization;
};

struct noncopyable
{
	noncopyable() { }
	noncopyable(const noncopyable &) = delete;
	noncopyable& operator=(const noncopyable &) = delete;
};

struct assembled_chunk {
	long unsigned int t0;
	int nt;
	unsigned char *data;
	char* buf;

	assembled_chunk(long int t0,int nt);

	~assembled_chunk();

	void set_data(int i, unsigned char x);

	inline void fill_efield_array_reference(std::uint8_t *efield, int *mask)
	{
	int arr_size = constants::nfreq * 2 * this->nt;

	for (int i = 0; i < arr_size; i++) {
		if (buf[i] != 0) {
		//offset_decode(re, im, buf[i]);
		efield[i] = buf[i];
		mask[i] = 1;
		}
		else {
		efield[i] = 0;
		mask[i] = 0;
		}
	}
	}
};

struct vdif_processor{
	std::string name;
	bool runflag;
	bool is_critical;
	pthread_mutex_t mutex;

	vdif_processor(const std::string &name_, bool is_critical_=false);

	virtual ~vdif_processor();

	bool is_running();

	void set_running();
	virtual void process_chunk(const assembled_chunk* a);
	virtual void finalize();
};

struct vdif_assembler {

	int number_of_processors;
	int start_index, end_index;
	int bufsize;
	short unsigned int port;

	unsigned char *data_buf;
	struct header *header_buf;

	vdif_processor **processors;
	std::thread *processor_threads;
	
	vdif_assembler(short unsigned int port);

	~vdif_assembler();

	int register_processor(vdif_processor *p);
	int kill_processor(vdif_processor *p);
	void run();
	void network_capture();
	void assemble_chunk();
	int is_full();
	void vdif_read(unsigned char *data, int size);
	void wait_until_end();
	void start_async();
};



}
