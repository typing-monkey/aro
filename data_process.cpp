using namespace std;

struct data_stream* create_stream(struct packet *packets) { 

	struct data_stream *stream = new struct data_stream[number_of_channels];

	for (int i = 0; i < number_of_channels; i++) {
		stream[i].stream = new struct polarization[int(max_packets / 2)];
	}

	for (int i = 0; i < max_packets; i++) {
		if (packets[i].head.threadID == 0) {
			for (int j = 0; j < number_of_channels; j++) {
				stream[j].stream[int(i/2)].x = packets[i].data[j];
			}
		}
		else {
			for (int j = 0; j < number_of_channels; j++) {
				stream[j].stream[int(i/2)].y = packets[i].data[j];
			}
		}
	}
	return stream;
}


float* square_sum(struct data_stream stream) {
	float *intensity = new float[sample_size];
	float temp = 0;

	for (int i = 0; i < max_packets / 2; i++) {
		temp += norm(stream.stream[i].x) + norm(stream.stream[i].y);
		if ((i + 1) % cadence == 0) {
			intensity[int(i / cadence)] = temp;
			temp = 0;					
		}
	}
	return intensity;
	

}