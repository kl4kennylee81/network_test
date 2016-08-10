
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
	int ec;
	printf("Sending %d bytes...\n", size);
	if ((ec = shmem_stream_send(stream, (char*) &size, sizeof(size)))) {
		return ec;
	}
	if ((ec = shmem_stream_send(stream, message, size))) {
		return ec;
	}
		
	int recv;
	if (( ec = shmem_stream_recv(stream, (char*) &recv, sizeof(recv)))) {
		return ec;
	}
	printf("Receiving %d bytes...\n", recv);
	if (recv < 1 || recv > 1024 * 1024) {
		return 1;
	}
	char* recvMsg = (char*) calloc (recv, 1);
	if ((ec = shmem_stream_recv(stream, recvMsg, recv))) {
		return ec;
	}

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
	stream.timeout = 3;
	int ec = shmem_stream_connect("/server", &stream);
	if (ec) {
		printf("Unable to connect to server!\n");
		return 1;
        }

	int i;
	for(i = 1; i < 1000; i++) {
		char buffer[5];
		sprintf(buffer, "%d", i);
		if ((ec = testPing(&stream, buffer))) {
			if (ec == SHMEM_ERR_TIMEOUT) {
				printf("Timeout! CLosing connection\n");
				shmem_stream_close(&stream);
				return ec;
			}
			printf("Shmem error %d\n", ec);
			return ec;
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
	printf("Client done\n");
	shmem_stream_close(&stream);
}
