
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "shmem_stream.h"

int testPing(shmem_stream_t* stream, char* message);

int testPing(shmem_stream_t* stream, char* message) {
	int size = strlen(message);

	printf("Sending %d bytes...\n", size);
	if (shmem_stream_send(stream, (char*) &size, sizeof(size))) {
		return 1;
	}
	if (shmem_stream_send(stream, message, size)) {
		return 1;
	}
		
	int recv;
	if (shmem_stream_recv(stream, (char*) &recv, sizeof(recv))) {
		return 1;
	}
	printf("Receiving %d bytes...\n", recv);
	if (recv < 1 || recv > 1024 * 1024) {
		return 1;
	}
	char* recvMsg = (char*) calloc (recv, 1);
	if (shmem_stream_recv(stream, recvMsg, recv)) {
		return 1;
	}

	printf("  received: %s\n", recvMsg);
	if (strncmp(message, recvMsg, recv)) {
		printf("  Bad response!\n");
		return 1;
	}
	free (recvMsg);
	return 0;
}

int main(int argc, char* argv[]) {

/*
	if (argc < 2)
		return 1;
*/
        shmem_stream_t stream;
	int ec = shmem_stream_connect("/server", &stream);
	if (ec) {
		printf("Unable to connect to server!\n");
        }

	int i;
	for(i = 1; i < 1000; i++) {
		char buffer[5];
		sprintf(buffer, "%d", i);
		if (testPing(&stream, buffer)) {
			break;
		}
	}

	char bigBuffer[8192] = {0};
	char* msg = "testing 123.";
	for (i = 0; i + strlen(msg) < sizeof(bigBuffer); ) {
		memcpy(&bigBuffer[i], msg, strlen(msg));
		i += strlen(msg);
	}
	

	for (i = 0; i < 10; i++) {
		if (testPing(&stream, bigBuffer)) {
			break;
		}
	}
	shmem_stream_close(&stream);
}
