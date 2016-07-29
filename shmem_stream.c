
#include "shmem_stream.h"

int shmem_stream_listen(char* name, shmem_stream_t* stream) {
  int ec = 0;
  strncpy(stream->name, name, MAX_SHM_KEY_LEN);
  int shmem_fd = shm_open(name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
  if (shmem_fd < 0) {
    printf("Error opening shared memory: %s\n", strerror(errno));
    return 1;
  }
  stream->fd = shmem_fd;

  int mem_lenth = sizeof(shmem_control_t);
  if ((ec = ftruncate(shmem_fd, mem_lenth))) {
      printf("Unable to resize memory to %d bytes: %s\n", mem_lenth, strerror(errno));
      return 1;
  }

  shmem_control_t* mem_control;
  mem_control = (shmem_control_t*) mmap(NULL, mem_lenth, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, shmem_fd, 0);

  if (mem_control == NULL) {
      printf("Unable to mmap memory: %s\n", strerror(errno));
      return 1;
  }
  stream->control = mem_control;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

  pthread_mutex_init(&mem_control->mutex, &attr);


  pthread_condattr_t condattr;
  pthread_condattr_init(&condattr);
  pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&mem_control->read_cond, &condattr);
  pthread_cond_init(&mem_control->write_cond, &condattr);

  pthread_mutex_lock(&mem_control->mutex);
  mem_control->ready = 1;
  pthread_mutex_unlock(&mem_control->mutex);
  return ec;
}

int shmem_stream_connect(char* name, shmem_stream_t* stream) {
	int ec;
        int shmem_fd = shm_open(name, O_RDWR, S_IRWXU | S_IRWXG);
	strncpy(stream->name, name, MAX_SHM_KEY_LEN);
        if (shmem_fd < 0) {
		printf("Unable to open shared memory: %s\n", strerror(errno));
                return 1;
        }
	stream->fd = shmem_fd;

        int mem_lenth = sizeof(shmem_control_t);

        shmem_control_t* mem_control;
        mem_control = (shmem_control_t*) mmap(NULL, mem_lenth, PROT_READ | PROT_WRITE,
                                              MAP_SHARED, shmem_fd, 0);

        if (mem_control == NULL) {
                printf("Unable to mmap memory: %s\n", strerror(errno));
                return 1;
        }
	stream->control = mem_control;

        if (!mem_control->ready) {
                printf("memory control block not ready!");
                return 1;
        } 
	return 0;
}

int shmem_stream_write(shmem_stream_t* stream, const char* buffer, size_t len) {
	pthread_mutex_lock(&stream->control->mutex);
	
	while (stream->control->length + len > MAX_BUFFER_LEN) {
		pthread_cond_wait(&stream->control->write_cond, &stream->control->mutex);
	}

	int cursor = stream->control->write_cursor;
	if (cursor + len > MAX_BUFFER_LEN) {
		printf("I can't write past end of buffer!");
		pthread_mutex_unlock(&stream->control->mutex);
		return 1;
	}
	memcpy(&stream->control->ring_buffer[cursor], buffer, len);
	cursor += len;
	stream->control->write_cursor = cursor;
	stream->control->length += len;
	pthread_cond_signal(&stream->control->read_cond);
	pthread_mutex_unlock(&stream->control->mutex);
	return 0;
}

int shmem_stream_read(shmem_stream_t* stream, char* buffer, size_t len) {
        char* peek;
	if (shmem_stream_peek(stream, &peek, len)) {
		return 1;
	}
	memcpy(buffer, peek, len);
	if (shmem_stream_advance(stream, len)) {
		return 1;
	}
	return 0;
}


int shmem_stream_peek(shmem_stream_t* stream, char** buffer, size_t len) {
	pthread_mutex_lock(&stream->control->mutex);

        while (stream->control->length < len) {
                pthread_cond_wait(&stream->control->read_cond, &stream->control->mutex);
        }
        if (len + stream->control->read_cursor > MAX_BUFFER_LEN) {
		printf("Oops. Can't read past end of buffer yet. Implement an iovec\n");
		pthread_mutex_unlock(&stream->control->mutex);
		return -1;
        }
	*buffer = &stream->control->ring_buffer[stream->control->read_cursor];
        pthread_mutex_unlock(&stream->control->mutex);  
	return 0;
}

int shmem_stream_advance(shmem_stream_t* stream, size_t len) {
	pthread_mutex_lock(&stream->control->mutex);
	int cursor = stream->control->read_cursor + len;
	if (cursor >= MAX_BUFFER_LEN) {
		cursor -= MAX_BUFFER_LEN;
	}
        if (cursor > stream->control->write_cursor) {
		printf("Oops. Can't advance past write cursor!\n");
		pthread_mutex_unlock(&stream->control->mutex);
		return 1;
        }
	if (len > stream->control->length) {
		printf("can't advance more than length!\n");
		pthread_mutex_unlock(&stream->control->mutex);
		return 1;
	}
	stream->control->length -= len;
	stream->control->read_cursor = cursor;
	pthread_cond_signal(&stream->control->write_cond);
	pthread_mutex_unlock(&stream->control->mutex);
	return 0;
}

int shmem_stream_shutdown(shmem_stream_t* stream) {
  shm_unlink(stream->name);
  close(stream->fd);
  return 0;
}

int shmem_stream_close(shmem_stream_t* stream) {
  close(stream->fd);
  return 0;
}
