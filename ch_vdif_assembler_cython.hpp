#include "vdif_assembler.hpp"

namespace vdif_assembler {
#if 0
}; // pacify emacs c-mode
#endif

struct base_python_processor: public vdif_processor {
	std::function<void(assembled_chunk*)> python_callback;
	base_python_processor(char* name, std::function<void(assembled_chunk*)> python_callback_) : vdif_processor(name){
		python_callback = python_callback_;
	}
	void initialize(){}
	void process_chunk(const assembled_chunk* a) {
		python_callback(a);
	}
	void finalize(){}
};

struct cpp_processor {
	vdif_processor* p;
	cpp_processor(const vdif_processor* p_){
		p = p_;
	}
};

struct cython_assembled_chunk {
	assembled_chunk* p;
	int64_t t0;
	int nt;

	cython_assembled_chunk(const assembled_chunk* p_) 
	: p(p_) 
	{
	xassert(p);
	this->t0 = p->t0;
	this->nt = p->nt;
	}

	// FIXME how to get a "float complex" pointer from cython?
	inline void fill_efield(std::uint8_t *efield, int32_t *mask)
	{
	if (!efield || !mask)
		throw std::runtime_error("NULL pointer passed to fill_efield()");

	p->fill_efield_array_reference(efield, mask);
	}
};

struct cython_assembler {
	vdif_assembler a;

	cython_assembler(bool write_to_disk, int rbuf_size, int abuf_size, int assembler_nt, int port)
	: a(port)
	{ }

	void register_cpp_processor(cpp_processor *processor)
	{
	a.register_processor(processor->p);
	}

	void start_async()
	{
	a.start_async();
	}

	void wait_until_end()
	{
	a.wait_until_end();
	}
};

}