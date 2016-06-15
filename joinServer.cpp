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
#include <iostream>

#define MAX_MESSAGE_SIZE 4096

using std::cout;
using std::endl;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

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
        cout << "Received " <<  std::dec << received << " bytes " << endl;

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

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"\nERROR, no port provided\n");
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
    while (1){
        newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        int count = 0;
        // loop back to accepting
        MPI_Comm intercom;
        MPI_Comm_join(newsockfd,&intercom);

        close(newsockfd);
        new_connection(intercom,count);
    }
    close(sockfd);

    /* Shutdown */
    MPI_Finalize();

    return 0;
}