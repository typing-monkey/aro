from multiprocessing import Process

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

import numpy as np
cimport numpy as np

cimport ch_vdif_assembler_pxd

##############################################  Constants  #########################################

chime_nfreq = ch_vdif_assembler_pxd.nfreq
timestamps_per_frame = ch_vdif_assembler_pxd.num_time

###########################################  assembled_chunk  ######################################


cdef class assembled_chunk:
	cdef ch_vdif_assembler_pxd.cython_assembled_chunk *p

	def __cinit__(self):
		self.p = NULL

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

cdef class cpp_processor:
	cdef ch_vdif_assembler_pxd.cpp_processor *p

	def __cinit__(self):
		self.p = NULL

	def __dealloc__(self):
		if self.p != NULL:
			del self.p
			self.p = NULL

##############################################  Assembler  #########################################


cdef class assembler:
	cdef ch_vdif_assembler_pxd.cython_assembler *p

	def __cinit__(self, bool write_to_disk, int rbuf_size, int abuf_size, int assembler_nt, int port):
		self.p = new ch_vdif_assembler_pxd.cython_assembler(write_to_disk, rbuf_size, abuf_size, assembler_nt, port)

	def __dealloc__(self):
		if self.p != NULL:
			del self.p
			self.p = NULL

	def register_cpp_processor(self, cpp_processor processor):
		self.p[0].register_cpp_processor(processor.p)

	def start_async(self):
		self.p[0].start_async()

	def wait_until_end(self):
		self.p[0].wait_until_end()


