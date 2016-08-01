
#include <pthread.h>
#include <stdlib.h>

#include "shmem_stream.h"


void handle_connection(shmem_stream_t* stream) {
	while (1) {
		int size;
		shmem_stream_recv(stream, (char*) &size, sizeof(size));
                char buffer[1024] = {0};
		printf("Reading %d bytes\n", size);
		if (size < sizeof(buffer)) {
			shmem_stream_recv(stream, buffer, size);
			printf("%s\n", buffer);
		}
		if (strcmp("shutdown", buffer) == 0)
			break;
	}
        shmem_stream_close(stream);
	free(stream);
}

int main(int argc, char* argv[]) {
	
        shmem_acceptor_t* acceptor;
	int ec = shmem_stream_listen("/server", &acceptor);
        if (ec) {
          printf("Unable to open server acceptor\n");
        }


	while (1) {
        	printf("Listening...\n");
		shmem_stream_t* stream = (shmem_stream_t*) malloc(sizeof(shmem_stream_t));
		ec = shmem_stream_accept(acceptor, stream);
		if (ec) {
		  printf("Unable to accept connection\n");
		  free(stream);
		  continue;
		}
		printf("Accepted connection %s!\n", stream->dest_control->name);
		pthread_t thread;
		pthread_create(&thread, NULL, (void* (*) (void*)) &handle_connection, stream);
	}
        shmem_stream_shutdown(acceptor);
        return 0;
}
