CC=g++

all: server client

server: server.cpp
	$(CC) server.cpp -o server `pkg-config ompi-c --cflags --libs`

client: client.cpp
	$(CC) client.cpp -o client `pkg-config ompi-c --cflags --libs`

clean:
	rm server client