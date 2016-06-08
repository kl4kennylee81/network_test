CC=g++
CFLAGS=-g -std=c++11 -pthread

all: server client

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server `pkg-config ompi-c ompi-cxx --cflags --libs`

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client `pkg-config ompi-c ompi-cxx --cflags --libs`

clean:
	rm server client