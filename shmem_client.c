
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "shmem_stream.h"

int main(int argc, char* argv[]) {


	if (argc < 2)
		return 1;

        shmem_stream_t stream;
	int ec = shmem_stream_connect("/server", &stream);
	if (ec) {
		printf("Unable to connect to server!\n");
        }

	int i;
	for(i = 1; i < argc; i++) {
		char* message = argv[i];
		int size = strlen(message);

		shmem_stream_send(&stream, (char*) &size, sizeof(size));
		shmem_stream_send(&stream, message, size);
		
		int recv;
		shmem_stream_recv(&stream, (char*) &recv, sizeof(recv));
		printf("Receiving %d bytes...\n", recv);
		char* recvMsg = (char*) calloc (recv, 1);
		shmem_stream_recv(&stream, recvMsg, recv);

		printf("  received: %s\n", recvMsg);
		if (strcmp(message, recvMsg)) {
			printf("  Bad response!\n");
		}
		free (recvMsg);
	}
	shmem_stream_close(&stream);
}
