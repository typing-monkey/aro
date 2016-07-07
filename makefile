CC:= g++ -mavx -std=c++11 -Wall -Wextra -O2


test: test.cpp vdif_assembler.hpp vdif_assembler.cpp square_sum.cpp
	$(CC) -o test test.cpp