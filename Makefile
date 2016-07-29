CC=g++
CFLAGS=-g -std=c++11 -pthread
TARGETS=server client singleton joinClient joinServer spawn child 

all: $(TARGETS) shmem_server

server: server.cpp
client: client.cpp
singleton: singleton.cpp
joinClient: joinClient.cpp
joinServer: joinServer.cpp
spawn: spawn.cpp
child: child.cpp
fork: fork.cpp

$(TARGETS):
	$(CC) $(CFLAGS) $< -o $@  -I/usr/local/include/ -L/usr/local/lib -lmpi -lmpi_cxx

SHMEM_SRC=shmem_server.c shmem_stream.c
SHMEM_OBJ=$(SHMEM_SRC:.c=.o)


.c.o:
	$(CC) $(FLAGS) -c $< -o $@

shmem_server: $(SHMEM_OBJ)
	$(CC) $(CFLAGS) $(SHMEM_OBJ) -o $@ -lrt

shmem_client: shmem_client.cpp
	$(CC) $(CFLAGS) $< -o $@ -lrt
clean:
	rm $(TARGETS) $(SHMEM_OBJ)
