#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <complex>
#include <cmath>
using namespace std;

struct header { int time, frame, threadID; };
struct packet { int size;  struct header head; complex<double> *data; };


void output_data(unsigned char *data, int size)
{
	int n_packet = int(size / 1056);
	struct packet *packets = new struct packet[n_packet];
	int word[8];
	unsigned char raw_data;
	int count = 0;
	int packet_count = 0;
	
	for (int i=0;i<32;i++){
		cout<<int(data[i])<<endl;
	}
	while (count < size) {
		for (int i = 0; i < 8; i++) {
			word[i] = (data[count+3] << 24) + (data[count+2] << 16) + (data[count+1] << 8) + data[count];
			count += 4;	
		}
		packets->head.time = word[0] & 0x3FFFFFFF;
		packets->head.frame = word[1] & 0xFFFFFF;
		packets->head.threadID = (word[3] >> 16) & 0x3FF;
		packets->size = (word[2] & 0xFFFFFF) * 8 - 32;
		cout << "size: " << packets->size << endl;
		packets->data = new complex<double>[packets->size];
		cout << "time: "<<packets->head.time << endl;
		cout << "frame: " << packets->head.frame << endl;
		cout << "threadID: " << packets->head.threadID << endl;
		for (int i = 0; i < packets->size; i++) {
			raw_data = data[count];
			packets->data[i] = complex<double>((raw_data >> 4) - 8, (raw_data & 0xF) - 8);
			count ++;
		}
		packets++;
		packet_count++;
		printf("Packets %d read.\n", packet_count);

	}
	//test
	int answer;
	packets = packets - n_packet;
	for (;;) {
		cout << "Give a packet number to test(start from 1), '0' to exit: " << endl;
		cin >> answer;
		if (answer != 0) {
			cout << "time: " << (packets + answer - 1)->head.time << '\n';
			cout << "frame: " << (packets + answer - 1)->head.frame << '\n';
			cout << "threadID: " << (packets + answer - 1)->head.threadID << '\n';
			cout << "data: ";
			for (int i = 0; i < (packets + answer - 1)->size; i++) {
				cout << (packets + answer - 1)->data[i] << ' ';
			}
			cout << endl;
		}
		else {
			break;
		}
	}
}
