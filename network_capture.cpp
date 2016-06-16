#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "test.cpp"

namespace vdif_network{
int main() {
	
	int port = 10050;
	int size = 1056 * 32;
	int bytes = 0;
	unsigned char dgram[size];
	
	int foo;
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
		bytes = read(sock_fd, dgram, sizeof(dgram));
		cout << bytes <<std::endl;
		output_data(dgram,bytes);
		cin >> foo;
		
	}

	


}
}

