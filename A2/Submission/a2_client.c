#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT 9000 //server listens on port 9000
   

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //creating the socket file descriptor

    if ( sockfd < 0 ) {         //error checking for socket creation
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    char *msg = "Time request"; //arbitrary message from client to be send to server


    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //instantiating with server info
       
    unsigned int n, len;   
    sendto(sockfd, (const char *)msg, strlen(msg), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); //sending message to server
    n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &servaddr,&len); // receiving server time in buffer
    buffer[n] = '\0'; //marking end of buffer string
    printf("Server time: %s\n", buffer);
   
    close(sockfd); //closing socket file descriptor
    return 0;
}