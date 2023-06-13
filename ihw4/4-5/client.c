#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <semaphore.h>
#include <time.h>


#define N 3
#define K 6
#define INVITATION_LEN 255

int main(int argc, char *argv[]) {
	srand(NULL);
	char* varirants[K] = {
		"Ресторан",
		"Кофе",
		"Кино",
		"Настольная игра",
		"Прогулка",
		"Поездка",
	};
	
	int sock;                        // Socket descriptor 
    struct sockaddr_in ServAddr; 
    unsigned short port;     
    char *servIP;                    // Server IP address (dotted quad)
    char* name;
    int type;
    socklen_t address_size = sizeof(ServAddr);
    
    char response[INVITATION_LEN];     // Buffer for echo string 
    unsigned int requestSize; 
    
    if (argc != 5)  // Test for correct number of arguments 
    {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n",
                argv[0]);
        exit(1);
    } 
    servIP = argv[1];             // First arg: server IP address (dotted quad)
    port = atoi(argv[2]); // Use given port, if any
    name = argv[3];
    type = atoi(argv[4]);
    
    // Create a reliable, stream socket using TCP
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        DieWithError("socket() failed");
    }
    
    ServAddr.sin_family = AF_INET;             // Internet address family
    ServAddr.sin_port = htons(port); // Server port
    
    // Establish the connection to the echo server
    if (inet_pton(AF_INET, servIP, &ServAddr.sin_addr) <= 0) {
        DieWithError("connect() failed\n");
    }
    
    char* request = varirants[type];

	printf("%s: %s\n", name, request);
    // Send the string to the server
    sendto(sock, request, INVITATION_LEN, 0, (struct sockaddr *)&ServAddr, address_size);

    if (recvfrom(sock, response, 41, 0, (struct sockaddr *)&ServAddr, &address_size) <= 0) {
        DieWithError("recv() failed or connection closed prematurely\n");
    }
    if (response[0] == '1') {
    	printf("%s: accepted\n", name);
    } else {
    	printf("%s: failed\n", name);
    }
    close(sock);
    exit(0);
	return 0;
}
