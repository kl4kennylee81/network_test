CC=gcc
CFLAGS=-g -pthread
CXX=g++
CXXFLAGS=-g -std=c++11 -pthread 
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
	$(CXX) $(CXXFLAGS) $< -o $@  -I/usr/local/include/ -L/usr/local/lib -lmpi -lmpi_cxx

SHMEM_SRC=shmem_server.c shmem_client.c shmem_parallel.c
SHMEM_OBJ=$(SHMEM_SRC:.c=.o)


.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

shmem_stream.o: shmem_stream.c
	$(CC) $(CFLAGS) -c $< -o $@ -lrt

shmem_server: shmem_stream.o shmem_server.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt

shmem_parallel: shmem_stream.o shmem_parallel.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt

shmem_client: shmem_stream.o shmem_client.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
clean:
	rm $(TARGETS) $(SHMEM_OBJ)
