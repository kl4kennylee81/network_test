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

shmem_server: shmem_server.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(TARGETS)
