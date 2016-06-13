CC:= g++ -Wall -Wextra -Wconversion

vdif_reader: vdif_reader.cpp vdif_reader.hpp data_process.cpp
	$(CC) -o vdif_reader vdif_reader.cpp