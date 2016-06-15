/**
 * OpenMPI Spawn Hosts test
 */
#include <mpi.h>
#include <unistd.h>
#include <iostream>

#define MAX_MESSAGE_SIZE 4096

using namespace std;

int main (int argc, char* argv[], char** envp) {

    /* Initilize execution environment */
    MPI_Init(&argc, &argv);
    
    int count = 100000;

    MPI_Comm intercom;
    int error_codes[1];
    char count_str[32];
    sprintf(count_str, "%d", count);

    char* _argv[2];
    _argv[0] = count_str;
    _argv[1] = NULL;

    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, "add-host", "localhost");

    // MPI_Comm_spawn("./client", _argv, 1, info, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);
    MPI_Comm_spawn("./child", _argv, 1, info, 0, MPI_COMM_SELF, &intercom, MPI_ERRCODES_IGNORE);

    cout << "Clients spawned " << endl;

    char message[MAX_MESSAGE_SIZE];
    int i;
    for (i=0; i < count; i++) {
        MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, intercom, NULL);
    }


    MPI_Comm_free(&intercom);

    /* Shutdown */
    MPI_Finalize();

    return 0;
}
