#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
   
#define PORT 9000 //server listens on port 9000
   

int main() {


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);  //creating the socket file descriptor
    if ( sockfd < 0 ) {                           //error checking for socket creation
        perror("socket creation failed");
        return -1;
    }


    char buffer[256];
    struct sockaddr_in servaddr, cliaddr;
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT); //instantiating with server info
       
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind error");
        return -1;
    }

    char *client_ip; //variable to store client ip address

    while(1) { //to ensure infinite acceptance of connections
        unsigned int client_len, n;
        client_len = sizeof(cliaddr);  
    
        n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), MSG_WAITALL, ( struct sockaddr *) &cliaddr,&client_len); //receiving data from client 
        buffer[n] = '\0'; //marking end of buffer string
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN); //using methods from  #include <arpa/inet.h> to store client IP address

        printf("Time server started:\n");
        printf("%s from: ", buffer); //printing the incoming message
        printf("%s:%d\n", client_ip, ntohs(cliaddr.sin_port)); //printing client IP Address and Port Number 
        
        time_t t;
        time(&t);
        char *server_time = ctime(&t);
        sendto(sockfd, (const char *)server_time, strlen(server_time), 0, (const struct sockaddr *) &cliaddr,client_len); //sending current server time to client
        
    }
    close(sockfd); //closing socket file descriptor
    return 0;
}