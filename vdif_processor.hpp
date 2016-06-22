#include <string>
#include <pthread.h>
#include <iostream>

namespace vdif_assembler {

struct assembler_chunk;
struct noncopyable;

struct vdif_processor{
	std::string name;
	bool runflag;
	bool is_critical;
	pthread_mutex_t mutex;

	vdif_processor(const std::string &name_, bool is_critical_=false);

	virtual ~vdif_processor();

	bool is_running();

	void set_running();
	virtual void process_chunk(const std::shared_ptr<assembler_chunk> &a);
	virtual void finalize();
};
}