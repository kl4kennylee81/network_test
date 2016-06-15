/**
 * OpenMPI Spawn Child
 */

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#define MAX_MESSAGE_SIZE 4096

using namespace std;

int main (int argc, char* argv[], char** envp) {

	MPI_Init(&argc, &argv);

	MPI_Comm parent;
	MPI_Comm_get_parent(&parent);

	int i;
	if (argc <= 1) {
		exit(1);
	}

	char* count_str = argv[1];
	int count = atoi(count_str);

	char message[MAX_MESSAGE_SIZE] = "Hello world!";
	for (i = 0; i < count; i++) {
	    MPI_Send(message, strlen(message), MPI_CHAR, 0, 0, parent);

		// this is recieving back the echo
	    // MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, server, NULL);
	}
	cout << "Sent " << i << endl;
	
	MPI_Finalize();
    return 0;
}
