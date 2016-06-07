/**
 * OpenMPI Echo Server Test Client
 */

#include <mpi.h>
#include <stdio.h>

using namespace std;

int main (int argc, char* argv[], char** envp) {

	char *port_name = NULL;
	size_t size;
	cout << "Enter port: ";
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

	const char message[] = "Hello world";
	server.Send(message, strlen(message) + 1, MPI::CHAR, 0, 0);

	MPI::Finalize();
    return 0;
}