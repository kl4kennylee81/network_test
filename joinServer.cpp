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

const char* shutdownCmd = "shutdown";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// created thread to handle the client
void static new_connection (MPI_Comm intercom, int count){
    while (1){
        char buffer[MAX_MESSAGE_SIZE] = {0};
        MPI_Status status;
        MPI_Recv(buffer, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, intercom, &status);

        // cout << buffer << endl;

        int received;
        MPI_Get_count(&status,MPI_CHAR, &received);

        if (strcmp(shutdownCmd, buffer) == 0) {
            cout << "Received shutdown on thread " << count << endl;
            return;
        }

        // send back the recieved message
        MPI_Send(buffer, received, MPI_CHAR, 0, 0,intercom);

    }

    MPI_Comm_free(&intercom);
}



int main (int argc, char* argv[]) {
    
    /* Initilize execution environment */
    // MPI::Init(argc, argv);

    /* Initialize a multithreaded environment */
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    MPI_Group serverGroup;
    MPI_Comm serverComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &serverComm);
    MPI_Comm_group(serverComm, &serverGroup);

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

    // loop back to accepting
    int count = 0;
    while (1) {
        newsockfd = accept(sockfd, 
             (struct sockaddr *) &cli_addr, 
             &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        std::cout << "Accepted connection " << count << std::endl;

        MPI_Comm intercom;
        MPI_Comm_join(newsockfd, &intercom);

        MPI_Group newGroup;
        MPI_Group group;
        MPI_Comm_group(intercom, &group);
        MPI_Group_union(group, serverGroup, &newGroup);
        serverGroup = newGroup;

        std::thread t1(new_connection, intercom, count);

        t1.detach();
        count++;
    }    // loop back to accepting


    close(newsockfd);
    close(sockfd);

    /* Shutdown */
    MPI_Finalize();

    return 0;
}