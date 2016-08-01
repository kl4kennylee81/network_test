
#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_stream_t stream;
	int ec = shmem_stream_listen("/server", &stream);
        if (ec) {
          printf("Unable to open server stream\n");
        }

        printf("Listening...\n");

	while (1) {
		int size;
		shmem_stream_read(&stream, (char*) &size, sizeof(size));
                char buffer[1024] = {0};
		printf("Reading %d bytes\n", size);
		if (size < sizeof(buffer)) {
			shmem_stream_read(&stream, buffer, size);
			printf("%s\n", buffer);
		}
		if (strcmp("shutdown", buffer) == 0)
			break;
	}
        shmem_stream_shutdown(&stream);
        return 0;
}
