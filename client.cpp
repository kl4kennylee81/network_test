/**
 * OpenMPI Echo Server Test Client
 */

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#define MAX_MESSAGE_SIZE 4096

using namespace std;

int main (int argc, char* argv[], char** envp) {

	MPI_Init(&argc, &argv);

	// char port_name[MPI_MAX_PORT_NAME];
	// MPI_Lookup_name("server", MPI_INFO_NULL, port_name);
    char* port_name = argv[1];

	cout << "Using port " << port_name << "\n";

	MPI_Comm server;
	MPI_Comm_connect(port_name,MPI_INFO_NULL,0,MPI_COMM_WORLD, &server);

	// size_t buf_size = MAX_MESSAGE_SIZE;
	// char message[MAX_MESSAGE_SIZE] = {0};
	// char* msg_ptr = message;
	// ssize_t bytes_read;
	// while (1){
	// 	cout << "Enter Msg: \n";
	// 	if ((bytes_read = getline(&msg_ptr, &buf_size, stdin)) == -1) {
	// 		cout << "Error reading from stdin\n";
	// 		return 1;
	// 	}
	// 	cout << "Sending bytes: " << bytes_read << endl;
	// 	server.Send(message, bytes_read, MPI::CHAR, 0, 0);

	// 	// this is recieving back the echo
	// 	memset(message, 0, bytes_read);
	//     server.Recv(message, MAX_MESSAGE_SIZE, MPI::CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG);
	//     cout << "Received: " << message << "\n";
	// }
	int i;
	int count = 10000;
	char message[MAX_MESSAGE_SIZE] = "Hello world!";
	for (i = 0; i < count; i++) {
	    MPI_Send(message, strlen(message), MPI_CHAR, 0, 0, server);

		// this is recieving back the echo
	    MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, server, NULL);
	}
	cout << "Sent " << i << endl;
	
	MPI_Finalize();
    return 0;
}
