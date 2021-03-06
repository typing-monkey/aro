include Makefile.local

CC:= g++ -Wall -Wextra -Wconversion

INCFILES= aro_vdif_assembler.hpp
LIBFILES=libvdif_assembler.so
LIBCYTHON=ch_vdif_assembler_cython.so
PYMODULES=ch_vdif_assembler.py

OFILES=aro_vdif_assembler.o
# vdif_reader: vdif_reader.cpp vdif_reader.hpp data_process.cpp
# 	$(CC) -o vdif_reader vdif_reader.cpp
#all: $(BINFILES) $(LIBFILES) $(LIBCYTHON) $(TESTBINFILES)

all: $(BINFILES) $(LIBFILES) $(LIBCYTHON) $(TESTBINFILES)

cython: $(LIBCYTHON)

%.o: %.cpp $(INCFILES)
	$(CPP) -c -o $@ $<

%_cython.cpp: %_cython.pyx ch_vdif_assembler_pxd.pxd ch_vdif_assembler_cython.hpp $(INCFILES)
	cython --cplus $<

libvdif_assembler.so: $(OFILES)
	$(CPP) -o $@ -shared $^

ch_vdif_assembler_cython.so: ch_vdif_assembler_cython.cpp libvdif_assembler.so aro_vdif_assembler.hpp
	$(CPP) -shared -o $@ $< -lvdif_assembler -lhdf5 -lpng

install: $(INCFILES) $(LIBFILES) $(LIBCYTHON)
	cp -f $(INCFILES) $(INCDIR)/
	cp -f $(LIBCYTHON) $(PYMODULES) $(PYDIR)/

uninstall:
	for f in $(INCFILES); do rm -f $(INCDIR)/$$f; done
	for f in $(LIBFILES); do rm -f $(LIBDIR)/$$f; done
	for f in $(LIBCYTHON) $(PYMODULES); do rm -f $(PYDIR)/$$f; done

clean:
	rm -f *~ *.o *_cython.cpp *.so *.pyc $(BINFILES) $(TESTBINFILES)