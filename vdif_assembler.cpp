
namespace aro_vdif_assembler{
#if 0
}; // pacify emacs c-mode
#endif

namespace constants{

}

struct vdif_processor{

};

vdif_assembler::vdif_assembler(int port, int nt, int chunk_timeout){

}



struct vdif_assembler
{
    vdif_assembler(int nt=);

    ~vdif_assembler();

    void register_processor(const std::shared_ptr<vdif_processor> &p);
    void run();

    void start_async();
    void wait_until_end();
};


struct vdif_processor : noncopyable {
    std::string name;
    bool is_critical;

    pthread_mutex_t mutex;
    bool runflag;

    //
    // If a processor is 'critical', then an exception thrown in the processor will kill the entire
    // assembler.  Otherwise, when the processor dies, the assembler (and any other registered processors)
    // keeps running until end-of-stream.
    // Alex - ignoring this flag
    //
    vdif_processor(const std::string &name, bool is_critical=false);
    virtual ~vdif_processor() { }

    bool is_running();
    void set_running();

    //
    // These are the member functions which must be instantiated in order to define a vdif_processor.
    //
    // The assembler calls process_chunk() multiple times, presenting the processor with the appearance
    // of a uniform sequence of assembled data.  When the stream ends, the assembler calls finalize().
    // See below for details on how to use the assembled_chunk!
    //
    virtual void process_chunk(const std::shared_ptr<assembled_chunk> &a) = 0;
    virtual void finalize() = 0;
};

}