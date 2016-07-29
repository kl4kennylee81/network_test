
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#include "shmem_control.h"

int main(int argc, char* argv[]) {
	
	int ec;
	int shmem_fd = shm_open(SHMEM_KEY, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);

	if (shmem_fd < 0) {
		std::cerr << "Unable to open shared memory" << std::endl;
		std::cerr << "  Error: " << strerror(errno) << std::endl;
		exit(1);
	}

	std::cout << "Using fd " << shmem_fd << std::endl;

	int mem_lenth = sizeof(shmem_control_t);
	if ((ec = ftruncate(shmem_fd, mem_lenth))) {
		std::cerr << "Unable to resize memory to " << mem_lenth << " bytes" << std::endl;
		std::cerr << "  Error: " << errno << ": " << strerror(errno) << std::endl;
		exit(1);
	}

	shmem_control_t* mem_control;
	mem_control = (shmem_control_t*) mmap(NULL, mem_lenth, PROT_READ | PROT_WRITE,
		                                MAP_SHARED, shmem_fd, 0);

	if (mem_control == NULL) {
		std::cerr << "Unable to mmap memory" << std::endl;
	}

	sleep(5);
	exit(0);
}