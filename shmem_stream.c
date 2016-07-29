
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

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

  pthread_mutex_init(&mem_control->mutex, &attr);


  pthread_condattr_t condattr;
  pthread_condattr_init(&condattr);
  pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&mem_control->ready_cond, &condattr);

  pthread_mutex_lock(&mem_control->mutex);
  mem_control->ready = 1;
  pthread_mutex_unlock(&mem_control->mutex);
  return ec;
}

int shmem_stream_connect(char* name, shmem_stream_t* stream) {
  
  return 0;
}

int shmem_stream_write(shmem_stream_t* stream, const char* buffer, size_t len) {
  return 0;
}

int shmem_stream_read(shmem_stream_t* stream, char* buffer, size_t len) {
  return 0;
}

int shmem_stream_advance(shmem_stream_t* stream, size_t len) {
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
