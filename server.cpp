/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>

using namespace std;

int main (int argc, char* argv[]) {
    
    /* Initilize execution environment */
    MPI::Init(argc, argv);

    int world_size = MPI::COMM_WORLD.Get_size();

    int world_rank = MPI::COMM_WORLD.Get_rank();

    char processor_name[MPI::MAX_PROCESSOR_NAME];
    int name_len;
    MPI::Get_processor_name(processor_name, name_len);

    cout << "Hello from processor " << processor_name <<
    	", rank " << world_rank << " of " <<  world_size << " processors\n";

    /* Shutdown */
    MPI::Finalize();

    return 0;
}