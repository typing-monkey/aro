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

	vdif_processor(const std::string &name_, bool is_critical_=false)
	: name(name_), is_critical(is_critical_), runflag(false){
		pthread_mutex_init(&mutex, NULL);
	}

	FIX
	virtual ~vdif_processor(){}

	bool is_running(){
		pthread_mutex_lock(&mutex);
		bool ret = runflag;
		pthread_mutex_unlock(&mutex);

		return ret;
	}

	void set_running(){
		pthread_mutex_lock(&mutex);

		if (runflag) {
		pthread_mutex_unlock(&mutex);
		throw_rerun_exception();
		}

		runflag = true;
		pthread_mutex_unlock(&mutex);
	}
	virtual void process_chunk(const std::shared_ptr<assembler_chunk> &a) = 0;
	virtual void finalize() = 0;
};
	
processor_handle::processor_handle(const string &name_, const shared_ptr<assembler_nerve_center> &nc_)
    : name(name_), nc(nc_),
      ichunk(-1),   // if ichunk is negative, then the first call to processor_get_chunk() will initialize
      ndrops(0), nprocessed(0)
{
    xassert(nc);
    nc->processor_start();
}


processor_handle::~processor_handle()
{
    nc->processor_end(ichunk);
    nc = shared_ptr<assembler_nerve_center> ();

    double dropfrac = 0.0;
    if ((ndrops > 0) || (nprocessed > 0))
	dropfrac = (double)ndrops / (double)(ndrops + nprocessed);
    
    stringstream ss;
    ss << name << ": " << nprocessed << " chunks processed, " 
       << ndrops << " dropped (dropfrac=" << dropfrac << ")\n";

    string s = ss.str();
    cout << s.c_str() << flush;
}


shared_ptr<assembled_chunk> processor_handle::get_next_chunk(thread_timer &timer)
{
    int nd = 0;
    shared_ptr<assembled_chunk> chunk = nc->processor_get_chunk(ichunk, nd, timer);

    if (chunk)
	nprocessed++;

    if (nd > 0) {
	cout << (string("  !!!! ") + name + " is running slow, can't keep up with assembler\n") << flush;
	ndrops += nd;
    }

    return chunk;
}

}