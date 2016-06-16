#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "vdif_reader.cpp"

using namespace std;

int network_capture(int port, struct header *header_buf, unsigned char *data_buf, int *start, int *end) {
	
	int size = 1056 * 32;

	unsigned char dgram[size];
	
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd < 0) {
		cout << "socket failed." << std::endl;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(sock_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		cout << "bind failed." <<std::endl;
	}
	for (;;) {
		if (read(sock_fd, dgram, sizeof(dgram)) == size) {
			vdif_read(dgram, bytes, header_buf, data_buf, start, end);
		}
		
	}

	


}
}

