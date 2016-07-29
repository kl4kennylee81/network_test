
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
	pthread_cond_t read_cond;
	pthread_cond_t write_cond;
	int length;
	int write_cursor;
        int read_cursor;
	char ring_buffer[MAX_BUFFER_LEN];
} shmem_control_t;

typedef struct {
	int fd;
	char name[MAX_SHM_KEY_LEN + 1];
	shmem_control_t* control;
} shmem_stream_t;

/* Setup shared memory control block */
int shmem_stream_listen(char* name, shmem_stream_t* stream);

/* Attempt to access and open shared memory control block*/
int shmem_stream_connect(char* name, shmem_stream_t* stream);

/* Copy len bytes of buffer into stream */
int shmem_stream_write(shmem_stream_t* stream, const char* buffer, size_t len);

/* Read and copy len bytes into buffer. Advances read cursor */
int shmem_stream_read(shmem_stream_t* stream, char* buffer, size_t len);

/* Points buffer to a region of memory with len bytes of data. Does not adanvce cursor
 * This is useful as a zero-copy mechanism. Must call 'advance' when done with memory */
int shmem_stream_peek(shmem_stream_t* stream, char** buffer, size_t len);

/* Advance cursor len bytes after calling 'peek' */
int shmem_stream_advance(shmem_stream_t* stream, size_t len);

/* Call from a server stream to unlink the shared memory segment */
int shmem_stream_shutdown(shmem_stream_t*);

/* Call from a client stream. Closes the file descriptor associated with the stream */
int shmem_stream_close(shmem_stream_t*);
