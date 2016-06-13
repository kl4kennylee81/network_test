CC=g++
CFLAGS=-g -std=c++11 -pthread
TARGETS=server client singleton

all: $(TARGETS)

server: server.cpp
client: client.cpp
singleton: singleton.cpp

$(TARGETS):
	$(CC) $(CFLAGS) $< -o $@ `pkg-config ompi-c ompi-cxx --cflags --libs`

clean:
	rm $(TARGETS)