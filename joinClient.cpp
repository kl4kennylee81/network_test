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

	MPI_Comm serverMPI;
    MPI_Comm_join(sockfd,&serverMPI);
    close(sockfd);

	size_t buf_size = MAX_MESSAGE_SIZE;
	char message[MAX_MESSAGE_SIZE] = {0};
	char* msg_ptr = message;
	ssize_t bytes_read;
	while (1){
		printf("Enter Msg:\n");
		if ((bytes_read = getline(&msg_ptr, &buf_size, stdin)) == -1) {
			printf("Error reading from stdin\n");
			return 1;
		}
		MPI_Send(message, bytes_read, MPI_CHAR, 0, 0,serverMPI);

		// this is recieving back the echo
		memset(message, 0, bytes_read);
        MPI_Status status;
	    MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,serverMPI,&status);
	    printf("Received: %s\n",message);
	}
	MPI_Finalize();
    return 0;
}