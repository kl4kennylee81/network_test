
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_LEN 4 * 48 * 1000 * 1000
#define MAX_SHM_KEY_LEN 255

typedef struct {
	volatile bool ready;
	pthread_mutex_t mutex;
	pthread_cond_t ready_cond;
	int length;
	int cursor;
	char ring_buffer[MAX_BUFFER_LEN];
} shmem_control_t;

typedef struct {
	int fd;
	char name[MAX_SHM_KEY_LEN + 1];
	shmem_control_t* control;
} shmem_stream_t;

int shmem_stream_listen(char* name, shmem_stream_t* stream);
int shmem_stream_connect(char* name, shmem_stream_t* stream);
int shmem_stream_write(shmem_stream_t* stream, const char* buffer, size_t len);
int shmem_stream_read(shmem_stream_t* stream, char* buffer, size_t len);
int shmem_stream_advance(shmem_stream_t* stream, size_t len);
int shmem_stream_shutdown(shmem_stream_t*);
int shmem_stream_close(shmem_stream_t*);
