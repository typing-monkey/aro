from libc.stdint cimport int32_t, int64_t
from libcpp.vector cimport vector
from libcpp cimport bool

from multiprocessing import Process

import numpy as np
cimport numpy as np

cimport ch_vdif_assembler_pxd
#from ch_vdif_assembler_pxd cimport cpp_processor
#from ch_vdif_assembler_pxd cimport processor_wrapper

##############################################  Constants  #########################################

chime_nfreq = ch_vdif_assembler_pxd.chime_nfreq
timestamps_per_frame = ch_vdif_assembler_pxd.timestamps_per_frame
#num_disks = ch_vdif_assembler_pxd.num_disks

############################################  cpp_processor  #######################################


#cdef class cpp_processor:
#	cdef ch_vdif_assembler_pxd.cpp_processor *p

#	def __cinit__(self):
#		self.p = NULL

#	def __dealloc__(self):
#		if self.p != NULL:
#			del self.p
#			self.p = NULL


#def cpp_waterfall_plotter(outdir, is_critical):
#    ret = cpp_processor()
#    ret.p = ch_vdif_assembler_pxd.cpp_waterfall_plotter(outdir, is_critical)
#    return ret


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

#cdef class test_assembler:
#	cdef int byte_rate
#	cdef cpp_processor proc
#	def __cinit__(self ,int byte_rate):
#		self.byte_rate = byte_rate
#	def __dealloc__(self):
#		if self.proc != NULL:
#			del self.proc
#			proc = NULL

#	def register_cpp_processor(self, cpp_processor proc):
#		self.proc = &proc

#	cdef serve_fake_chunk():
#		pass

#	def start_async(self):
#		pass


#cdef class processor_wrapper:
#	def __init__(self,proc):
#		self.p =  <cpp_processor*>proc

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

	#def register_python_processor(self):
	#	self.p[0].register_python_processor()

	#fix?
	#def get_next_python_chunk(self):
	#	cdef ch_vdif_assembler_pxd.cython_assembled_chunk *a = self.p[0].get_next_python_chunk()

	#	if a == NULL:
	#		return None
 
	#	ret = assembled_chunk()
	#	ret.p = a
	#	return ret

	#def unregister_python_processor(self):
	#	self.p[0].unregister_python_processor()

	def start_async(self):
		self.p[0].start_async()

	def wait_until_end(self):
		self.p[0].wait_until_end()

#cdef class cython_processor(cpp_processor):
#	cdef <void*> python_proc
#	def __cinit__(self, <void*> python_proc):
#		self.python_proc = python_proc

#	cdef 

#cdef class cpp_processor:
#	#std::shared_ptr<vdif_processor> p;
#	#cpp_processor(const std::shared_ptr<vdif_processor> &p_) : p(p_) { xassert(p); }
#	cdef vdif_processor* p
#	cdef object* call
#	cdef has_python_call = False
#	def __cinit__(self, vdif_processor p_):
#		self.p = &p_

#	cdef set_python_call(self, void* call):
#		self.call = <object> call
#		self.has_python_call = True

#	cdef process_python_chunk(self, assembler_chunk* chunk):
#		self.call(chunk)

#	cdef process_chunk(self, assembler_chunk* chunk):
#		if self.has_python_call:
#			self.process_python_chunk(chunk)
#		self.process_c_chunk(chunk)

#	cdef process_c_chunk(self, assembler_chunk* chunk):
#		pass

#class processor(cpp_processor):
#	"""
#	To define a python processor, you subclass this base class.
#	When the assembler runs, it will call process_chunk() with a sequence of chunks, represented
#	by a (t0,nt,efield,mask) quadruple.
#	Each chunk corresponds to range of timestamps [t0,t0+nt), where t0 is a 64-bit wraparound-free
#	timestamp.
#	WARNING 1: Usually these ranges will be contiguous between calls, e.g.
#		[t0,t0+nt)   [t0+nt,t0+2*nt)   [t0+2*nt,t0+3*nt)   ...
#	but the vdif_processor should not assume that this!  If there is a temporary 
#	interruption in data stream, then a timestamp gap will appear.
#	The 'efield' arg is a shape (nfreq,2,nt) complex array with electric field values, where
#	the middle index is polarziation.  Missing data is represented by (0+0j).  The 'mask' arg
#	is a shape (nfreq,2,nt) integer array which is 0 for missing data, and 1 for non-missing.
#	 WARNING 2: Handling missing data is an important aspect of the vdif_processor since it 
#	 happens all the time.  If a GPU correlator node is down, which is a frequent occurrence, 
#	 then some frequencies will be "all missing".  There are also routine packet loss events 
#	 on second-timescales which result in some high-speed samples being flagged as missing data.
#	 """

#	def python_process_chunk(self, t0, nt, efield, mask):
#		print 'process_chunk called! t0=%s nt=%s efield (%s,%s) mask (%s,%s)' % (t0, nt, efield.dtype, efield.shape, mask.dtype, mask.shape)

#	def finalize(self):
#		pass

