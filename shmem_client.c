
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_stream_t stream;
	int ec = shmem_stream_connect("/server", &stream);
	if (ec) {
		printf("Unable to connect to server!\n");
        }

	if (argc < 2)
		return 1;

	char* message = argv[1];
	int size = strlen(message);

	shmem_stream_write(&stream, (char*) &size, sizeof(size));
	shmem_stream_write(&stream, message, size);
	
	shmem_stream_close(&stream);
}
