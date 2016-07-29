
#include "shmem_stream.h"

int main(int argc, char* argv[]) {
	
        shmem_stream_t stream;
	int ec = shmem_stream_listen("/server", &stream);
        if (ec) {
          printf("Unable to open server stream\n");
        }

        printf("Listening...\n");
/*
	pthread_mutex_lock(&mem_control->mutex);
        std::cout << "Lock acquired" << std::endl;
        while (mem_control->length == 0) {
		pthread_cond_wait(&mem_control->ready_cond, &mem_control->mutex);
        }
	std::cout << "Received: " << mem_control->queue[mem_control->cursor].data << std::endl;
        ++mem_control->cursor;
        --mem_control->length;
	pthread_mutex_unlock(&mem_control->mutex);
*/
        shmem_stream_shutdown(&stream);
        return 0;
}
