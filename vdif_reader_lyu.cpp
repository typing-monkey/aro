#include "vdif_reader.hpp"
#include "data_process.cpp"
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("run with:%s <filelist_name>\n", argv[0]);
		exit(1);
	}
	ifstream fl (argv[1], ifstream::in);
	string fn[10];
	int num_files = 0;
	while (getline(fl, fn[num_files])) {
		num_files++;
	}

	fl.close();
	FILE *fp[num_files];
	for (int i = 0; i < num_files - 1; i++) {
		fn[i] = fn[i].substr(0, fn[i].size() - 1);
	}

	for (int i = 0; i < num_files; i++) {
		fp[i] = fopen(fn[i].c_str(), "r");
		if (!fp[i]) {
			cout << i << endl;
			printf("can't open file\n");
			exit(10);
		}
	}
	int length = int(max_packets * num_files / cadence / 2);

	struct freq_band *bands = new struct freq_band[number_of_channels];

	for (int i = 0; i < number_of_channels; i++) {
		bands[i].bandID = i + 1;
		bands[i].size = 0;
		bands[i].intensity = new float[length];
	}

	struct packet *packets = new struct packet[max_packets];
	for (int i = 0; i < max_packets; i++) {
		packets[i].data = new complex<float>[number_of_channels];
	}

	int word[8];
	unsigned char raw_data;
	int count = 0;
	struct data_stream *stream = new struct data_stream[number_of_channels];

	for (int i = 0; i < number_of_channels; i++) {
		stream[i].stream = new struct polarization[int(max_packets / 2)];
	}

	for (int f = 0; f < num_files; f++) {

		cout << "Reading file " << f+1 << endl;

		while (!feof(fp[f]) && count < max_packets) {
			//read header
			for (int i = 0; i < 8; i++) {
				fread(&word[i], sizeof(word[i]), 1, fp[f]);
			}
			packets[count].head.time = word[0] & 0x3FFFFFFF;
			packets[count].head.frame = word[1] & 0xFFFFFF;
			packets[count].head.threadID = (word[3] >> 16) & 0x3FF;

			//read data
			for (int i = 0; i < number_of_channels; i++) {
				fread(&raw_data, sizeof(raw_data), 1, fp[f]);
				packets[count].data[i] = complex<float>(float((raw_data >> 4) - 8), float((raw_data & 0xF) - 8));
			}

			//next packet
			count++;
		}

		if (count > max_packets) {
			count = max_packets;
		}


		create_stream(packets, stream, count);
		

		cout << count << " packets read." << endl;


		for (int i = 0; i < number_of_channels; i++) {
			square_sum(stream[i], bands[i].intensity, &(bands[i].size), &(bands[i].corr));
		
		}
		count = 0;

		fclose(fp[f]);

	}


	for (int i = 0; i < max_packets; i++) {
		delete[] packets[i].data;
	}
	delete[] packets;

	for (int i = 0; i < number_of_channels; i++) {
		delete[] stream[i].stream;
	}
	delete[] stream;


	//test
	for (int i = 0; i < 10; i++) {
		cout << "Frequency " << bands[i].bandID << endl;
		for (int j = 0; j < length; j++) {
			cout << bands[i].intensity[j] << ' ';
		}
		cout << endl;
	}
	cout << length << endl;
	cout << bands[0].size << endl;

}


