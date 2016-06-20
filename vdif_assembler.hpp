#include <iostream>
#include <stdlib.h>

#ifndef _unlikely
#define _unlikely(cond)  (__builtin_expect(cond,0))
#endif

#ifndef xassert
#define xassert(cond) xassert2(cond, __LINE__)
#define xassert2(cond,line) \
    do { \
        if (_unlikely(!(cond))) { \
	    const char *msg = "Assertion '" __STRING(cond) "' failed (" __FILE__ ":" __STRING(line) ")\n"; \
	    std::cout << msg << std::flush; \
	    throw std::runtime_error(msg); \
	} \
    } while (0)
#endif

namespace vdif_assembler{

struct header {
	long int t0;
	int polarization;
};

struct noncopyable
{
    noncopyable() { }
    noncopyable(const noncopyable &) = delete;
    noncopyable& operator=(const noncopyable &) = delete;
};

struct assembled_chunk {
	
	long int t0;
	unsigned char *data;

	assembled_chunk(long int t0);

	~assembled_chunk();

	void set_data(int i, unsigned char x);

};


struct vdif_processor {

	bool is_alive;
	vdif_processor();
	~vdif_processor();
	void process_chunk(assembled_chunk *c);

};



struct vdif_assembler {

	int number_of_processors;
	int start_index, end_index;
	int bufsize;

	unsigned char *data_buf;
	struct header *header_buf;

	vdif_processor **processors;

	vdif_assembler();

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