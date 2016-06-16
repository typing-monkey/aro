#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>
#include <cmath>
using namespace std;


void vdif_read(unsigned char *data, int size, struct header *header_buf, unsigned char *data_buf, int *start, int *end) {


	int word[8];
	int count = 0;
	int temp = 0;

	while (count < size) {
		for (int i = 0; i < 8; i++) {
			word[i] = (data[count+3] << 24) + (data[count+2] << 16) + (data[count+1] << 8) + data[count];
			count += 4;	
		}
		header_buf[*end].t0 = long int ((word[0] & 0x3FFFFFFF) * constants::frame_per_second + (word[1] & 0xFFFFFF));
		header_buf[*end].polarization = (word[3] >> 16) & 0x3FF;
		temp = (*end) * constants::nfreq;
		for (int i = 0; i < constants::nfreq; i++) {
			data[temp] = data[count];
			temp++;
			count++;
		}
		(*end)++;

	}

}
