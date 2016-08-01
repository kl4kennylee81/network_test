
#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_acceptor_t* acceptor;
	int ec = shmem_stream_listen("/server", &acceptor);
        if (ec) {
          printf("Unable to open server acceptor\n");
        }

        printf("Listening...\n");
	shmem_stream_t stream;
	ec = shmem_stream_accept(acceptor, &stream);
	if (ec) {
	  printf("Unable to accept connection\n");
	}
	printf("Accepted connection!\n");

	while (1) {
		int size;
		shmem_stream_recv(&stream, (char*) &size, sizeof(size));
                char buffer[1024] = {0};
		printf("Reading %d bytes\n", size);
		if (size < sizeof(buffer)) {
			shmem_stream_recv(&stream, buffer, size);
			printf("%s\n", buffer);
		}
		if (strcmp("shutdown", buffer) == 0)
			break;
	}
        shmem_stream_close(&stream);
        shmem_stream_shutdown(acceptor);
        return 0;
}
