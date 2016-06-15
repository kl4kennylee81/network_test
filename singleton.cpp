/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define MAX_MSG 4096

int main (int argc, char* argv[], char** envp) {

    /* Initilize execution environment */
    MPI_Init(&argc, &argv);
    
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    cout << "Rank " << world_rank << endl;
    int count = 10000;
    if (world_rank == 0) {
        char port_name[MPI_MAX_PORT_NAME];
        MPI_Open_port(MPI_INFO_NULL, port_name);

        MPI_Publish_name("server", MPI_INFO_NULL, port_name);

        cout << "Accepting on " << port_name << "\n";

        MPI_Comm intercom;
        MPI_Comm_accept(
            port_name,
            MPI_INFO_NULL,
            0,
            MPI_COMM_SELF,
            &intercom);

        cout << "Accepted connection!" << "\n";

        char buffer[MAX_MSG];
        int i;
        for (i=0; i<count; i++) {
            MPI_Status status;
            MPI_Recv(buffer, MAX_MSG, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, intercom, &status);

        }
  
        MPI_Comm_free(&intercom);
        MPI_Close_port(port_name);

    } else {

        usleep(100 * 1000);
        char port_name[MPI_MAX_PORT_NAME];
        MPI_Lookup_name("server", MPI_INFO_NULL, port_name);

        cout << "Connecting to " << port_name << "...\n";
        MPI_Comm server;
        MPI_Comm_connect(
            port_name,
            MPI_INFO_NULL,
            0,
            MPI_COMM_SELF,
            &server);
        cout << "Connected!\n";   

        int i;
        const char message[MAX_MSG] = "Hello world";
        for (i=0; i<count; i++) {

            MPI_Send(message, MAX_MSG, MPI_CHAR, 0, 0, server);
        }

    }

    /* Shutdown */
    MPI_Finalize();

    return 0;
}
