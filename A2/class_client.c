#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>

int main()
{
    //socket()
    int serverfd = socket(AF_INET,SOCK_DGRAM,0);
    if(serverfd<0)
    {
        perror("socket error");
        return -1;
    }
    if(setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
    {
        perror("setsockopt error");
        exit(-1);
    }
 
    struct sockaddr_in srvaddr;
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(5000); //htonl
    srvaddr.sin_addr.s_addr = INADDR_ANY;

    while(1)
    {
        char buffer[256];
        scanf("%s",buffer);
        sendto(serverfd,buffer,sizeof(buffer),0,(struct sockaddr*)&srvaddr,sizeof(srvaddr));
        if(strcmp(buffer,"exit")==0)
        break;
    }
    close(serverfd);
}