from libc.stdint cimport int32_t, int64_t, uint8_t

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
cimport numpy as np

cdef extern from "vdif_assembler.cpp" namespace "vdif_assembler::constants":
        int nfreq

cdef extern from "ch_vdif_assembler_cython.hpp" namespace "vdif_assembler":
    cdef cppclass cython_stream:
        pass

    cdef cppclass cpp_processor:
        pass    # opaque to cython

    cdef cppclass cython_assembled_chunk:
        long t0
        int nt

        void fill_efield(uint8_t *efield, int32_t *mask) except +


    cdef cppclass cython_assembler:
        cython_assembler(bool write_to_disk, int rbuf_size, int abuf_size, int assembler_nt, int port) except +

        void register_cpp_processor(cpp_processor *p) except +
        #void register_python_processor() except +
        #void unregister_python_processor() except +
        void start_async() except +
        void wait_until_end() except +

        cython_assembled_chunk *get_next_python_chunk() except +


    # Factory functions
    #cython_stream *cython_file_stream(const vector[string] &filename_list) except +
    #cython_stream *cython_simulated_stream(double gbps, double nsec) except +
    #cython_stream *cython_network_stream() except +
    #cpp_processor *cpp_waterfall_plotter(const string &outdir, bool is_critical) except +