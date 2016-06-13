#include "vdif_reader.hpp"
#include "data_process.cpp"
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("run with:%s <filename>\n", argv[0]);
		exit(1);
	}
	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		printf("can't open file\n");
		exit(10);
	}

	struct packet *packets = new struct packet[max_packets];
	int word[8];
	unsigned char raw_data;
	int count = 0;

	while (!feof(fp)) {

		//read header
		for (int i = 0; i < 8; i++) {
			fread(&word[i], sizeof(word[i]), 1, fp);
		}
		packets[count].head.time = word[0] & 0x3FFFFFFF;
		packets[count].head.frame = word[1] & 0xFFFFFF;
		packets[count].head.threadID = (word[3] >> 16) & 0x3FF;
		packets[count].data = new complex<float>[number_of_channels];

		//read data
		for (int i = 0; i < number_of_channels; i++) {
			fread(&raw_data, sizeof(raw_data), 1, fp);
			packets[count].data[i] = complex<float>(float((raw_data >> 4) - 8), float((raw_data & 0xF) - 8));
		}

		//next packet
		count++;
		printf("Packet %d read.\n", count);
	}
	
	fclose(fp);
	//test
	int answer;
	for (;;) {
		cout << "Give a packet number to test(start from 1), '0' to exit: " << endl;
		cin >> answer;
		if (answer != 0) {
			cout << "time: " << (packets + answer - 1)->head.time << '\n';
			cout << "frame: " << (packets + answer - 1)->head.frame << '\n';
			cout << "threadID: " << (packets + answer - 1)->head.threadID << '\n';
			cout << "data: ";
			for (int i = 0; i < number_of_channels; i++) {
				cout << (packets + answer - 1)->data[i] << ' ';
			}
			cout << endl;
		}
		else {
			break;
		}
	}
	//struct data_stream *stream = create_stream(packets);
	//struct freq_band *bands = new struct freq_band[number_of_channels];

	//for (int i = 0; i < number_of_channels; i++) {
	//	bands[i].bandID = i + 1;
	//	bands[i].intensity = new float[sample_size];
	//	bands[i].intensity = square_sum(stream[i]);
	//}


	////test
	//for (int i = 0; i < 10; i++) {
	//	cout << "Frequency :" << bands[i].bandID << endl;
	//	for (int j = 0; j < sample_size; j++) {
	//		cout << bands[i].intensity[j] << ' ';
	//	}
	//	cout << endl;
	//}

}