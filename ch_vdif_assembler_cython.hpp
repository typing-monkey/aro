#ifndef _CH_VDIF_ASSEMBLER_CYTHON_HPP
#define _CH_VDIF_ASSEMBLER_CYTHON_HPP

#include "aro_vdif_assembler.hpp"

namespace aro_vdif_assembler{
#if 0
}; // pacify emacs c-mode
#endif
// struct base_python_processor : public vdif_processor {
// 	void (*python_callback)(cython_assembled_chunk*);
// 	base_python_processor(char* name, void (*python_callback_)(cython_assembled_chunk*)) : vdif_processor(name){
// 		python_callback = python_callback_;
// 	}
// 	~base_python_processor(){
// 		//TODO
// 	}
// 	virtual void initialize();
// 	virtual void process_chunk(const cython_assembled_chunk* a);
// 	virtual void finalize();
// };



// void base_python_processor::process_chunk(const cython_assembled_chunk* a){
// 	python_callback(a);
// }

struct cpp_processor {
	vdif_processor* p;
	cpp_processor(const vdif_processor* p_){
		p = p_;
	}
};

// struct source_cpp_processor{
// 	base_python_processor* p;
// 	source_cpp_processor(const base_python_processor* p_){
// 		p = p_;
// 	}
// };

struct cython_assembled_chunk {
	assembled_chunk* p;
	int64_t t0;
	int nt;

	cython_assembled_chunk(assembled_chunk* p_) 
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

// inline source_cpp_processor *cpp_python_processor(const void(*python_callback)(assembled_chunk*))
// {
//     vdif_processor* p = make_python_processor(python_callback);
//     return new cpp_processor(p);
// }

struct cython_assembler {
	vdif_assembler a;

	cython_assembler(char* arg1, char*arg2)
	: a(arg1,arg2)
	{ }

	void register_cpp_processor(cpp_processor *processor)
	{
	a.register_processor(processor->p);
	}

	cython_assembled_chunk* get_chunk(){
		return new cython_assembled_chunk(a.get_chunk());
	}

	void start_async()
	{
	a.start_async();
	}

	void use_network(){
		a.set_source_type(0);
		a.set_args("network","10050");
	}

	void use_simulate(){
		a.set_source_type(1);
	}

	void use_filelist(){
		a.set_source_type(2);
	}

	void wait_until_end()
	{
		a.wait_until_end();
	}
};

}

#endif // _CH_VDIF_ASSEMBLER_CYTHON_HPP