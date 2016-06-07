/**
 * OpenMPI Echo Server Test Client
 */

#include <mpi.h>
#include <stdio.h>

using namespace std;

int main (int argc, char* argv[], char** envp) {

	char *port_name = NULL;
	size_t size;
	cout << "Enter port: \n";
	if (getline(&port_name, &size, stdin) == -1) {
		cout << "Error reading from stdin\n";
		return 1;
	}

	MPI::Init(argc, argv);

	// char port_name[MPI_MAX_PORT_NAME];
	// MPI::Lookup_name("server", MPI::INFO_NULL, port_name);
	cout << "Using port " << port_name << "\n";

	MPI::Intracomm server = MPI::COMM_WORLD.Connect(port_name,
		MPI::INFO_NULL,
		0);
	while (1){
		char *message = NULL;
		size_t msg_size;
		cout << "Enter Msg: \n";
		if (getline(&message, &msg_size, stdin) == -1) {
			cout << "Error reading from stdin\n";
			return 1;
		}
		printf("%lu\n",msg_size);
		server.Send(message, msg_size, MPI::CHAR, 0, 0);

		// this is recieving back the echo
		char buffer[32] = {0};
	    server.Recv(buffer, 32, MPI::CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG);
	    cout << "Received: " << buffer << "\n";
	}
	MPI::Finalize();
    return 0;
}