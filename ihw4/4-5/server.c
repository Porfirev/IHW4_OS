#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

#define N 3
#define INVITATION_LEN 255

void DieWithError(char *errorMessage);  // Error handling function

sem_t *sem;
int already_going = 0;
int was = 0;
typedef struct {
    int valentine_type_num[N];
    int lucky;
} Memory;


void ClientHandle(int socket, struct sockaddr_in ClntAddr) {
	unsigned int clntLen = sizeof(ClntAddr);
    sem_wait(sem);
    char request[INVITATION_LEN];

    /* Receive message from client */
    if ((recvfrom(socket, request, INVITATION_LEN, 0, (struct sockaddr *) &ClntAddr, &clntLen)) < 0)
        DieWithError("recv() failed");

	char res[2];
	res[0] = '0';
    if (already_going == 0) {
    	int test = (rand() % (N - was));
    	if (test == 0) {
    		already_going = 1;
    		res[0] = '1';
    	}
    }
    /* Message response to client */
    sendto(socket, res, 2, 0, (struct sockaddr *) &ClntAddr, clntLen);
    sem_post(sem);
    was++;
}


int main(int argc, char *argv[]) {
	srand(NULL);
    int servSock;                    /* Socket descriptor for server */
    int clntSock[N];                    /* Socket descriptor for client */
    struct sockaddr_in ServAddr; /* Local address */
    struct sockaddr_in ClntAddr; /* Client address */
    unsigned short ServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    socklen_t cli_address_size = sizeof(ServAddr);
    
	pid_t children[N];
    sem_t lsem;
    sem = &lsem;
    sem_init(sem, 0, 1);
    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }
    
    ServPort = atoi(argv[1]);
    
    if ((servSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    	DieWithError("socket() failed");
    
    ServAddr.sin_family = AF_INET;                /* Internet address family */
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(ServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
        DieWithError("bind() failed");

    printf("Server IP address = %s. Wait...\n", inet_ntoa(ServAddr.sin_addr));

	for (int i = 0; i < N; ++i) {
		ClientHandle(servSock, ClntAddr);
	}
	printf("finish\n");
	return 0;
}
