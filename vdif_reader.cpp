#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

const int n_freq = 1024;
const int n_pol = 2;
const int max_packets = 131072;

int header[max_packets][8];
unsigned char data[n_freq][n_pol][max_packets >> 2] = { {{0}} };

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("run with:%s <file_name>\n", argv[0]);
		exit(1);
	}

	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		printf("can't open file\n");
		exit(10);
	}
	
	int pol;

	for (int i = 0; (i < max_packets) && !(feof(fp)); i++) {
		for (int j = 0; j < 8; j++) {
			fread(&header[i][j],sizeof(header[i][j]),1,fp);
		}
		pol = (header[i][3]>>16) & 0x3FF;
		for (int j = 0; j < n_freq; j++) {
			fread(&data[j][pol][int(i/2)], sizeof(data[j][pol][int(i/2)]), 1, fp);
		}
		cout << "Reading packet " << i << endl;
	}
	int start_time = header[0][0] & 0x3FFFFFFF;
	int start_frame = header[0][1] & 0xFFFFFF;
	int end_time, end_frame;
	for (int i = 131071; i > 0; i--) {
		end_time = header[i][0] & 0x3FFFFFFF;
		if (end_time > 0) {
			end_frame = header[i][1] & 0xFFFFFF;
			break;
		}
	}

	//test
	int freq, po, time;
	for (;;) {
		cout << "Frequency Polarizaiton Time: ";
		cin >> freq;
		cin >> po;
		cin >> time;
		if (po > 1) {
			break;
		}
		cout << int(data[freq][po][time]) << endl;
	}
}