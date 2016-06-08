/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>
#include <thread>

using namespace std;


//
// created thread to handle the client
void static new_connection (MPI::Intercomm intercom, int count){
    cout << "Accepted connection on thread " << count << "\n";

    while (1){
        char buffer[32] = {0};
        cout << "Waiting for data on thread " << count << endl;
        intercom.Recv(buffer, 32, MPI::CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG);
        cout << "Received (" << strlen(buffer) << " bytes): " << buffer << "\n";

        // send back the recieved message
        intercom.Send(buffer, strlen(buffer) + 1, MPI::CHAR, 0, 0);
        cout << "im here\n";
    }

    intercom.Free();
}



int main (int argc, char* argv[]) {
    
    /* Initilize execution environment */
    // MPI::Init(argc, argv);

    /* Initialize a multithreaded environment */
    int provided = MPI::Init_thread(argc, argv, MPI::THREAD_MULTIPLE);
    if (provided < MPI::THREAD_MULTIPLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI::COMM_WORLD.Abort(1);
    }

    int world_size = MPI::COMM_WORLD.Get_size();

    int world_rank = MPI::COMM_WORLD.Get_rank();

    char processor_name[MPI::MAX_PROCESSOR_NAME];
    int name_len;
    MPI::Get_processor_name(processor_name, name_len);

    cout << "Hello from server process " << processor_name <<
    	", rank " << world_rank << " of " <<  world_size << " processors\n";

    char port_name[MPI_MAX_PORT_NAME];
    MPI::Open_port(MPI::INFO_NULL, port_name);

    cout << "Port name: " << port_name << "\n";

    MPI::Publish_name("server", MPI::INFO_NULL, port_name);

    int count = 0;
    // loop back to accepting
    while (1) {
        cout << "Waiting for client " << count << endl;
        MPI::Intercomm intercom = MPI::COMM_SELF.Accept(port_name,MPI::INFO_NULL,0);
        std::thread t1(new_connection,intercom,count);

        t1.detach();
        count++;
    }

    MPI::Close_port(port_name);

    /* Shutdown */
    MPI::Finalize();

    return 0;
}