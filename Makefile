CC=mpic++
CFLAGS=-g

all: server client

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server `pkg-config ompi-c --cflags --libs`

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client `pkg-config ompi-c --cflags --libs`

clean:
	rm server client