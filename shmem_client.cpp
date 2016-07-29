
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

#include "shmem_control.h"

int main(int argc, char* argv[]) {
	
	int ec;
	int shmem_fd = shm_open(SHMEM_KEY, O_RDWR, S_IRWXU | S_IRWXG);

	if (shmem_fd < 0) {
		std::cerr << "Unable to open shared memory" << std::endl;
		std::cerr << "  Error: " << strerror(errno) << std::endl;
		exit(1);
	}

	std::cout << "Using fd " << shmem_fd << std::endl;

	int mem_lenth = sizeof(shmem_control_t);

	shmem_control_t* mem_control;
	mem_control = (shmem_control_t*) mmap(NULL, mem_lenth, PROT_READ | PROT_WRITE,
		                                MAP_SHARED, shmem_fd, 0);

	if (mem_control == NULL) {
		std::cerr << "Unable to mmap memory" << std::endl;
		exit(1);
	}

	if (!mem_control->ready) {
		std::cerr << "memory control block not ready!";
		exit(1);
        }

	pthread_mutex_lock(&mem_control->mutex);
        std::cout << "Lock acquired" << std::endl;
        if (mem_control->length < MAX_QUEUE_LEN) {
		char* mymsg = "hello world!";
		int cursor = mem_control->cursor;
		strcpy(mem_control->queue[cursor].data, mymsg);
		mem_control->queue[cursor].len = strlen(mymsg);
                ++mem_control->length;
		pthread_cond_signal(&mem_control->ready_cond);
		std::cout << "Sent message!" << std::endl;
        }
	pthread_mutex_unlock(&mem_control->mutex);

        close(shmem_fd);
}
