#include "vdif_assembler.cpp"
#include <string>
#include <pthread.h>
#include <iostream>

namespace vdif_assembler {

struct vdif_processor : noncopyable{
	std::string name;
	bool runflag;
	bool is_critical;
	pthread_mutex_t mutex;

	vdif_processor(const std::string &name_, bool is_critical_=false);

	virtual ~vdif_processor();

	bool is_running();

	void set_running()';
	virtual void process_chunk(const std::shared_ptr<assembler_chunk> &a);
	virtual void finalize();
};
}