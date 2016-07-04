CC:= g++ -std=c++11 -Wall -Wextra -Wconversion -O2


test: test.cpp vdif_assembler.hpp vdif_assembler.cpp
	$(CC) -o test test.cpp