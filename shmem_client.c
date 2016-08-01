
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_stream_t stream;
	int ec = shmem_stream_connect("/server", &stream);
	if (ec) {
		printf("Unable to connect to server!\n");
        }

	if (argc < 2)
		return 1;

	int i;
	for(i = 1; i < argc; i++) {
		char* message = argv[i];
		int size = strlen(message);

		shmem_stream_send(&stream, (char*) &size, sizeof(size));
		shmem_stream_send(&stream, message, size);
		
		int recv;
		char* recvMsg = (char*) malloc (recv);
		shmem_stream_recv(&stream, (char*) &recv, sizeof(recv));
		shmem_stream_recv(&stream, recvMsg, recv);

		printf("Received: %s\n", recvMsg);
		if (strcmp(message, recvMsg)) {
			printf("  Bad response!\n");
		}
	}
	shmem_stream_close(&stream);
}
