/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>
#include <unistd.h>

using namespace std;

int main (int argc, char* argv[], char** envp) {

    /* Initilize execution environment */
    MPI::Init();

    int world_size = MPI::COMM_WORLD.Get_size();
    int world_rank = MPI::COMM_WORLD.Get_rank();

    char processor_name[MPI::MAX_PROCESSOR_NAME];
    int name_len;
    MPI::Get_processor_name(processor_name, name_len);

    cout << "Hello from server process " << processor_name <<
    	", rank " << world_rank << " of " <<  world_size << " processors\n";

    if (world_rank == 0) {
        char port_name[MPI_MAX_PORT_NAME];
        MPI::Open_port(MPI::INFO_NULL, port_name);

        MPI::Publish_name("server", MPI::INFO_NULL, port_name);

        cout << "Accepting on " << port_name << "\n";

        MPI::Intercomm intercom = MPI::COMM_SELF.Accept(
            port_name,
            MPI::INFO_NULL,
            0);

        cout << "Accepted connection!" << "\n";

        char buffer[32] = {0};
        intercom.Recv(buffer, 32, MPI::CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG);
        cout << "Received: " << buffer << "\n";

        intercom.Free();
        MPI::Close_port(port_name);

    } else {

        sleep(1);
        char port_name[MPI_MAX_PORT_NAME];
        MPI::Lookup_name("server", MPI::INFO_NULL, port_name);

        cout << "Connecting to " << port_name << "...\n";
        MPI::Intracomm server = MPI::COMM_SELF.Connect(
            port_name,
            MPI::INFO_NULL,
            0);

        cout << "Connected!\n";   

        const char message[] = "Hello world";
        server.Send(message, strlen(message) + 1, MPI::CHAR, 0, 0);

    }

    /* Shutdown */
    MPI::Finalize();

    return 0;
}