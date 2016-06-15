CC=g++
CFLAGS=-g -std=c++11 -pthread
TARGETS=server client singleton joinClient joinServer

all: $(TARGETS)

server: server.cpp
client: client.cpp
singleton: singleton.cpp
joinClient: joinClient.cpp
joinServer: joinServer.cpp

$(TARGETS):
	$(CC) $(CFLAGS) $< -o $@ `pkg-config mpich --cflags --libs`

clean:
	rm $(TARGETS)
