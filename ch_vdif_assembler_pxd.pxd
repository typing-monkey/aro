

from libc.stdint cimport int32_t, int64_t, uint8_t

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
#from libc.std cimport function
cimport numpy as np

cdef extern from "aro_vdif_assembler.hpp" namespace "aro_vdif_assembler::constants":
        int nfreq
        int chunk_size
        
cdef extern from "aro_vdif_assembler.hpp" namespace "aro_vdif_assembler":
        cdef cppclass assembled_chunk:
            long unsigned int t0
            int nt
            #char* buf
            unsigned char* data
            void fill_efield_array_reference(np.uint8_t *efield, int *mask)
            void set_data(int i, unsigned char x)

cdef extern from "ch_vdif_assembler_cython.hpp" namespace "aro_vdif_assembler":
    #cdef cppclass cython_stream:
     #   pass

    #cdef cppclass cpp_processor:
    #    pass    # opaque to cython
    #cdef cppclass source_cpp_processor:
    #    assembled_chunk* get_chunk()

    #cdef cppclass base_python_processor:
    #    base_python_processor(char* name)

    cdef cppclass cython_assembled_chunk:
        long t0
        int nt
        cython_assembled_chunk(assembled_chunk* p_) 
        void fill_efield(uint8_t *efield, int32_t *mask) except +

    cdef cppclass cython_assembler:
        cython_assembler(bool write_to_disk, int rbuf_size, int abuf_size, int assembler_nt, int port) except +

        #void register_cpp_processor(cpp_processor *p) except +
        #void register_python_processor() except +
        #void unregister_python_processor() except +
        void start_async() except +
        void wait_until_end() except +
        cython_assembled_chunk* get_chunk() except +

    #cpp_processor *cpp_python_processor() except +

#cdef class cp_assembled_chunk:
#    cdef cython_assembled_chunk *p


# refactor
#        cython_assembled_chunk *get_next_python_chunk() except +

#cdef class processor_wrapper:
#    cdef cpp_processor *p
#    def __init__(self,proc)
    #processor_wrapper(cpp_processor proc)

    # Factory functions
    #cython_stream *cython_file_stream(const vector[string] &filename_list) except +
    #cython_stream *cython_simulated_stream(double gbps, double nsec) except +
    #cython_stream *cython_network_stream() except +
    #cpp_processor *cpp_waterfall_plotter(const string &outdir, bool is_critical) except +