#ifndef _ARO_VDIF_ASSEMBLER_HPP
#define _ARO_VDIF_ASSEMBLER_HPP

#if (__cplusplus < 201103) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error "This source file needs to be compiled with C++0x support (g++ -std=c++0x)"
#endif

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <cinttypes>

#include "emmintrin.h"
#include "tmmintrin.h"
#include "smmintrin.h"

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



namespace aro_vdif_assembler{
#if 0
}; // pacify emacs c-mode
#endif

namespace constants{

	static const int nfreq = 1024;
	static const int header_size = 12; //int64 time + int32 thread ID
	static const int chunk_size = 65536;
	static const int num_time = 1024;
	static const int max_processors = 10;
	static const int frame_per_second = 390625;
	static const int buffer_size = 524288; //at most 8 chunk in buffer
	static const int packets_per_file = 131072;
	static const int udf_packetsize = 1056 * 32;

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
	//char* buf;

	assembled_chunk(long int t0,int nt);

	~assembled_chunk();

	void set_data(int i, unsigned char x);

	inline void fill_efield_array_reference(std::uint8_t *efield, int *mask)
	{
	int arr_size = constants::nfreq * 2 * this->nt;

	for (int i = 0; i < arr_size; i++) {
		if (data[i] != 0) {
		//offset_decode(re, im, buf[i]);
		efield[i] = data[i];
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
	virtual void process_chunk(const assembled_chunk* a) = 0;
	virtual void finalize() = 0;
};

struct vdif_assembler {

	int number_of_processors;
	int start_index, end_index;
	int bufsize;
	int port;
	std::string source;
	char* filelist_name;

	unsigned char *temp_buf;
	unsigned char *data_buf;
	struct header *header_buf;
	std::queue<assembled_chunk*> chunks;

	vdif_processor **processors;
	std::thread *processor_threads;
	
	vdif_assembler(const char*arg1, const char *arg2);

	~vdif_assembler();

	int register_processor(vdif_processor *p);
	int kill_processor(vdif_processor *p);
	void run();
	void network_capture();
	void read_from_disk();
	void assemble_chunk();
	int is_full();
	assembled_chunk* get_chunk();
	void vdif_read(unsigned char *data, int size);
	void wait_until_end();
	void start_async();
	void fill_missing(int n);

};

struct base_python_processor : public vdif_processor {
	//void (*python_callback)(assembled_chunk*);
	//base_python_processor(char* name, void (*python_callback_)(assembled_chunk*));
	base_python_processor(char* name);
	~base_python_processor();
	virtual void initialize();
	virtual void process_chunk(const assembled_chunk* a);
	virtual void finalize();
	assembled_chunk* get_chunk();
};

base_python_processor* make_python_processor(void (* callback)(assembled_chunk*));

inline void _sum16_auto_correlations(int &sum, int &count, const uint8_t *buf)
{
    __m128i x = _mm_loadu_si128(reinterpret_cast<const __m128i *> (buf));
    __m128i mask_invalid = _mm_cmpeq_epi8(x, _mm_set1_epi8(0));
    
    // take "horizontal" sum of mask to get the count
    __m128i s = _mm_andnot_si128(mask_invalid, _mm_set1_epi8(1));
    s = _mm_add_epi8(s, _mm_srli_si128(s,1));
    s = _mm_add_epi8(s, _mm_srli_si128(s,2));
    s = _mm_add_epi8(s, _mm_srli_si128(s,4));
    s = _mm_add_epi8(s, _mm_srli_si128(s,8));
    count = _mm_extract_epi8(s, 0);

    // replace invalid bytes by 0x88
    x = _mm_andnot_si128(mask_invalid, x);
    x = _mm_or_si128(x, _mm_and_si128(mask_invalid, _mm_set1_epi8(0x88)));

    // Each of the next 4 stanzas computes eight 16-bit numbers

    __m128i xim_lo = _mm_and_si128(x, _mm_set1_epi16(0x000f));
    xim_lo = _mm_sub_epi16(xim_lo, _mm_set1_epi16(0x0008));
    __m128i xim2_lo = _mm_mullo_epi16(xim_lo, xim_lo);

    __m128i xim_hi = _mm_and_si128(x, _mm_set1_epi16(0x0f00));
    xim_hi = _mm_sub_epi16(xim_hi, _mm_set1_epi16(0x0800));
    __m128i xim2_hi = _mm_mulhi_epi16(xim_hi, xim_hi);

    __m128i xre_lo = _mm_and_si128(x, _mm_set1_epi16(0x00f0));
    xre_lo = _mm_srli_epi16(xre_lo, 4);
    xre_lo = _mm_sub_epi16(xre_lo, _mm_set1_epi16(0x0008));
    __m128i xre2_lo = _mm_mullo_epi16(xre_lo, xre_lo);

    __m128i xre_hi = _mm_and_si128(x, _mm_set1_epi16(0xf000));
    xre_hi = _mm_srli_epi16(xre_hi, 12);
    xre_hi = _mm_sub_epi16(xre_hi, _mm_set1_epi16(0x0008));
    __m128i xre2_hi = _mm_mullo_epi16(xre_hi, xre_hi);

    __m128i xim2 = _mm_add_epi16(xim2_lo, xim2_hi);
    __m128i xre2 = _mm_add_epi16(xre2_lo, xre2_hi);
    __m128i x2 = _mm_add_epi16(xre2, xim2);

    // "horizontal" sum of 8 16-bit integers
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,2));
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,4));
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,8));
    sum = _mm_extract_epi16(x2, 0);
}

}

#endif // _ARO_VDIF_ASSEMBLER_HPP