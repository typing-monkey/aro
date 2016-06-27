from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

import numpy as np
cimport numpy as np

cimport ch_vdif_assembler_pxd

from ch_vdif_assembler_pxd cimport cython_assembled_chunk, cython_assembler

##############################################  Constants  #########################################

chime_nfreq = ch_vdif_assembler_pxd.nfreq

#cdef class cpp_processor:
#	cdef ch_vdif_assembler_pxd.cpp_processor *p

#	def __cinit__(self):
#		self.p = NULL

#	def __dealloc__(self):
#		if self.p != NULL:
#			del self.p
#			self.p = NULL

###########################################  assembled_chunk  ######################################

cdef class cp_assembled_chunk:
	cdef cython_assembled_chunk *p

	def __cinit__(self):
		self.p = NULL

	#def __init__(self):
	#	pass
		#self.p = <ch_vdif_assembler_pxd.cython_assembled_chunk*>p

	def __dealloc__(self):
		if self.p != NULL:
			del self.p
			self.p = NULL

	def get_data(self):
		if self.p == NULL:
			return None

		t0 = self.p[0].t0
		nt = self.p[0].nt

		cdef np.ndarray[np.uint8_t,ndim=3,mode='c'] efield = np.zeros((chime_nfreq,2,nt),dtype=np.uint8)
		cdef np.ndarray[np.int32_t,ndim=3,mode='c'] mask = np.zeros((chime_nfreq,2,nt),dtype=np.int32)

		self.p[0].fill_efield(&efield[0,0,0], &mask[0,0,0])
		return (t0, nt, efield, mask)



#cdef class assembled_chunk:
#    cdef ch_vdif_assembler_pxd.cython_assembled_chunk *p

#    def __cinit__(self):
#        self.p = NULL

#    def __dealloc__(self):
#        if self.p != NULL:
#            del self.p
#            self.p = NULL

#    def get_data(self):
#        if self.p == NULL:
#            return None

#        t0 = self.p[0].t0
#        nt = self.p[0].nt

#        cdef np.ndarray[np.complex64_t,ndim=3,mode='c'] efield = np.zeros((chime_nfreq,2,nt),dtype=np.complex64)
#        cdef np.ndarray[np.int32_t,ndim=3,mode='c'] mask = np.zeros((chime_nfreq,2,nt),dtype=np.int32)

#        self.p[0].fill_efield(&efield[0,0,0], &mask[0,0,0])
#        return (t0, nt, efield, mask)

#cdef void convert_to_cython_chunk(assembled_chunk* c, void* fun):
#	cython_assembled_chunk* a = new cython_assembled_chunk(<assembled_chunk*>c)
#	fun(a)



	#ret.p = new ch_vdif_assembler_pxd.cython_assembled_chunk(<ch_vdif_assembler_pxd.assembled_chunk*>c)
#	return ret
#def convert_chunk(x)

#cdef void convert_chunk_logic(assembled_chunk* x):



#cdef convert_python_call(c_call, python_call):
#	return lambda x: python_call(convert_to_python_chunk(<void*>c_call(x)))



#def cpp_python_processor(python_callback, is_critical=False):
#	ret = cpp_processor()
#	callback = lambda x: convert_to_cython_chunk(x,<void*>python_callback)
#	ret.p = ch_vdif_assembler_pxd.cpp_python_processor(<void*>callback)
#	return ret

#def cpp_python_processor():
#	ret = cpp_processor()
#	ret.p = ch_vdif_assembler_pxd.cpp_python_processor()
#	return ret

#cdef class python_processor:
#	cdef ch_vdif_assembler_pxd.base_python_processor *p
#	def __cinit__(self):
#		fun = (lambda x: self.process_chunk(x))
#		self.p = new ch_vdif_assembler_pxd.base_python_processor(name, <void*>fun)

#	cpdef process_chunk(self, np.uint8_t* efield):
#		pass
#	#def process_chunk(self, t0, nt, efield, mask):
#	#	pass

##############################################  Assembler  #########################################


cdef class assembler:
	cdef ch_vdif_assembler_pxd.cython_assembler *p

	def __cinit__(self, bool write_to_disk, int rbuf_size, int abuf_size, int assembler_nt, int port):
		self.p = new ch_vdif_assembler_pxd.cython_assembler(write_to_disk, rbuf_size, abuf_size, assembler_nt, port)

	def __dealloc__(self):
		if self.p != NULL:
			del self.p
			self.p = NULL

	#def get_python_processor(self):
	#	cpp_proc = cpp_python_processor()
	#	self.register_cpp_processor(cpp_proc)
	#	return cpp_proc

	def get_chunk(self):
		cdef cp_assembled_chunk chunk = cp_assembled_chunk()
		chunk.p = self.p[0].get_chunk()
		return chunk

	#def register_cpp_processor(self, cpp_processor processor):
	#	self.p[0].register_cpp_processor(processor.p)

	def start_async(self):
		self.p[0].start_async()

	def wait_until_end(self):
		self.p[0].wait_until_end()


