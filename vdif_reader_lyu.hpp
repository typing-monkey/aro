#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <complex>
#include <math.h>
#include <string>

using namespace std;

const int max_packets = 131072;
const int number_of_channels = 1024;
const int cadence = 384;
const int sample_size = int(max_packets / cadence / 2);

struct header { int time, frame, threadID; };
struct packet { struct header head; complex<float> *data; };

struct polarization { complex<float> x, y; };
struct data_stream {struct polarization *stream; };
struct freq_band { int bandID, size, corr; float *intensity; };