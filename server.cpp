/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>
#include <iostream>
#include <thread>

#define MAX_MESSAGE_SIZE 4096

using namespace std;


//
// created thread to handle the client
void static new_connection (MPI_Comm intercom, int count) {
    std::cout << "Accepted connection on thread " << count << "\n";

    int i;
    for (i=0; i < 10000; i++){
        char buffer[MAX_MESSAGE_SIZE] = {0};

        MPI_Status status;
        MPI_Recv(buffer, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, intercom, &status);

        int received;
        MPI_Get_count(&status, MPI_CHAR, &received);
        // cout << "Received " <<  std::dec << received << " bytes " << endl;

        if (received >= 16) {
            if (buffer[12] == '\xd4' && buffer[13] == '\x07') {
                buffer[12] = 1;
                buffer[13] = 0;
            }
        }


        // send back the recieved message
        MPI_Send(buffer, received, MPI_CHAR, 0, 0, intercom);

    }

    std::cout << "Done" << std::endl;

    MPI_Comm_free(&intercom);
}



int main (int argc, char* argv[]) {
    
    /* Initilize execution environment */
    // MPI::Init(argc, argv);

    /* Initialize a multithreaded environment */

	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	if (provided < MPI_THREAD_MULTIPLE)
	{
	    printf("ERROR: The MPI library does not have full thread support\n");
	    MPI_Abort(MPI_COMM_WORLD, 1);
	}

    /* Duplicate the global communicator, get its group, add new clients to the global group */
    MPI_Group serverGroup;
    MPI_Comm serverComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &serverComm);
    MPI_Comm_group(serverComm, &serverGroup);


    char port_name[MPI_MAX_PORT_NAME];
    MPI_Open_port(MPI_INFO_NULL, port_name);

    cout << "Port name: " << port_name << endl;

    MPI_Publish_name("server", MPI_INFO_NULL, port_name);

    int count = 0;
    while (1) {
        MPI_Comm intercom;
        MPI_Group group;
        MPI_Comm_accept(port_name,MPI_INFO_NULL,0, serverComm, &intercom);

        /* Add the new communicator's group to our global group. */
        MPI_Group newGroup;
        MPI_Comm_group(intercom, &group);
        MPI_Group_union(group, serverGroup, &newGroup);
        serverGroup = newGroup;

        new_connection(intercom, count);
        std::thread t1(new_connection,intercom,count);

        t1.detach();
        count++;
    }

    MPI_Close_port(port_name);

    /* Shutdown */
    MPI_Finalize();

    return 0;
}
