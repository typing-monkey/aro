import os
import re
import sys
import errno
import numpy as np

import ch_vdif_assembler_cython

from ch_vdif_assembler_cython import cpp_python_processor

class constants:
	chime_nfreq = ch_vdif_assembler_cython.chime_nfreq
	timestamps_per_frame = ch_vdif_assembler_cython.timestamps_per_frame
	#num_disks = ch_vdif_assembler_cython.num_disks


class assembler:
	def __init__(self, port=1000, write_to_disk=False, rbuf_size=0, abuf_size=4, assembler_nt=65536):
		self._assembler = ch_vdif_assembler_cython.assembler(write_to_disk, rbuf_size, abuf_size, assembler_nt, port)
		self.python_processor = None

	def register_processor(self, p):
		if isinstance(p, ch_vdif_assembler_cython.cpp_processor):
			self._assembler.register_cpp_processor(p)   # register C++ processor (this actually spawns a processing thread)
		elif not isinstance(p, processor):
			raise RuntimeError('Argument to assembler.register_processor() must be either an object of class ch_vdif_assembler.processor, or a C++ processor (e.g. returned by make_waterfall_plotter)')
		elif self.python_processor is not None:
			raise RuntimeError('Currently, ch_vdif_assembler only allows registering one python processor (but an arbitrary number of C++ processors)')
		else:
			self.python_processor = p

	#fix?
	def run(self, stream):
		# if self.python_processor is None:
		# 	self._assembler.start_async()
		# 	self._assembler.wait_until_end()
		# 	return
		self._assembler.start_async()
		while True:
			(t0,nt,efield,mask) = self._assembler.get_chunk().get_data()
			self.python_processor.process_chunk(t0,nt,efield,mask)

		# self._assembler.register_python_processor()

		# try:
		# 	self._assembler.start_async()
		
		# 	while True:
		# 		chunk = self._assembler.get_next_python_chunk()
		# 		if chunk is None:
		# 			break
		# 		(t0, nt, efield, mask) = chunk.get_data()
		# 		self.python_processor.process_chunk(t0, nt, efield, mask)

		# 	self.python_processor.finalize()

		# finally:
		# 	self._assembler.unregister_python_processor()

		self._assembler.wait_until_end()

class processor:
	"""
	To define a python processor, you subclass this base class.
	When the assembler runs, it will call process_chunk() with a sequence of chunks, represented
	by a (t0,nt,efield,mask) quadruple.
	Each chunk corresponds to range of timestamps [t0,t0+nt), where t0 is a 64-bit wraparound-free
	timestamp.
	WARNING 1: Usually these ranges will be contiguous between calls, e.g.
		[t0,t0+nt)   [t0+nt,t0+2*nt)   [t0+2*nt,t0+3*nt)   ...
	but the vdif_processor should not assume that this!  If there is a temporary 
	interruption in data stream, then a timestamp gap will appear.
	The 'efield' arg is a shape (nfreq,2,nt) complex array with electric field values, where
	the middle index is polarziation.  Missing data is represented by (0+0j).  The 'mask' arg
	is a shape (nfreq,2,nt) integer array which is 0 for missing data, and 1 for non-missing.
	 WARNING 2: Handling missing data is an important aspect of the vdif_processor since it 
	 happens all the time.  If a GPU correlator node is down, which is a frequent occurrence, 
	 then some frequencies will be "all missing".  There are also routine packet loss events 
	 on second-timescales which result in some high-speed samples being flagged as missing data.
	 """	
	def process_chunk(self, t0, nt, efield, mask):
		print 'process_chunk called! t0=%s nt=%s efield (%s,%s) mask (%s,%s)' % (t0, nt, efield.dtype, efield.shape, mask.dtype, mask.shape)



	def finalize(self):
		pass

# class singleton_python_processor(processor):
# 	self._assembler
# 	def __init__(self,py_assembler):
# 		self._assembler = py_assembler
# 		#self.cpp_processor = py_assembler.get_python_processor()
# 	def get_and_process_chunk():
# 		chunk = self._assembler.get_chunk()




