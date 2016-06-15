/**
 * OpenMPI Echo Server Test
 */
#include <mpi.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_MESSAGE_SIZE 4096

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// created thread to handle the client
void static new_connection (MPI::Intercomm intercom, int count){
    while (1){
        char buffer[MAX_MESSAGE_SIZE] = {0};
        MPI::Status status = MPI::Status();
        intercom.Recv(buffer, MAX_MESSAGE_SIZE, MPI::CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, status);

        printf(buffer);

        int received = status.Get_count(MPI::CHAR);

        // send back the recieved message
        intercom.Send(buffer, received, MPI::CHAR, 0, 0);

    }

    intercom.Free();
}



int main (int argc, char* argv[]) {
    
    /* Initilize execution environment */
    // MPI::Init(argc, argv);

    /* Initialize a multithreaded environment */
    int provided = MPI::Init_thread(argc, argv, MPI::THREAD_MULTIPLE);

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if ((bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)) {
        error("ERROR on binding");
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
             (struct sockaddr *) &cli_addr, 
             &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    int count = 0;
    // loop back to accepting
    MPI::Intercomm intercom = MPI::COMM_SELF.Join(newsockfd);
    new_connection(intercom,count);

    /* Shutdown */
    MPI::Finalize();

    return 0;
}