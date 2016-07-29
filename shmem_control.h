
#include <pthread.h>
#include <unistd.h>

#define MAX_MESSAGE_LEN 48 * 1000
#define MAX_QUEUE_LEN 4
#define SHMEM_KEY "/message_queue"

typedef struct {
	size_t len;
	char data[MAX_MESSAGE_LEN];
} shmem_message_t;

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t ready_cond;
	int length;
	int cursor;
	shmem_message_t queue[MAX_QUEUE_LEN];
} shmem_control_t;

