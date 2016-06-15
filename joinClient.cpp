/**
 * OpenMPI Echo Server Test Client
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAX_MESSAGE_SIZE 4096

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main (int argc, char* argv[], char** envp) {

    MPI_Init(&argc, &argv);

	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"\nusage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    printf("Connected to the socket\n");

	MPI_Comm serverMPI;
    MPI_Comm_join(sockfd,&serverMPI);

    printf("joined the herd\n");
    close(sockfd);

    int i;
    int count = 10;
    char message[MAX_MESSAGE_SIZE] = "Hello world!";
    for (i = 0; i < count; i++) {
		MPI_Send(message, strlen(message), MPI_CHAR, 0, 0,serverMPI);

        MPI_Status status;
	    MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,serverMPI,&status);
	    // printf("Received: %s\n",message);
	}

    printf("hi im done\n");

    MPI_Abort(serverMPI,0);
	MPI_Finalize();
    printf("finish finalize\n");
    return 0;
}