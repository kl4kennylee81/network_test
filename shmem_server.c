
#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_stream_t stream;
	int ec = shmem_stream_listen("/server", &stream);
        if (ec) {
          printf("Unable to open server stream\n");
        }

        printf("Listening...\n");

	while (1) {
		char c;
		shmem_stream_read(&stream, &c, 1);
		printf("%c\n", c);
	}
        shmem_stream_shutdown(&stream);
        return 0;
}
