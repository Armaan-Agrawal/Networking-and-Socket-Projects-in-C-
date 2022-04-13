//client

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>

#include<unistd.h>
#include<stdlib.h>

#define PORT 6000 //server listens on port 6000


int main()
{
	//socket
	int server_sd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sd<0)
	{
		perror("socket:");
		exit(-1);
	}
	//setsock
	int value  = 1;
	setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value)); //&(int){1},sizeof(int)
	
    
    struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY, INADDR_LOOP

	//connect
    if(connect(server_sd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        perror("connect");
        exit(-1);
    }
	
	//accept
    printf("Connected to server \n");
	
    
	while(1)
	{  
       char buffer[256]; //to store the message entered by user
       bzero(buffer,sizeof(buffer));

       char server_response[256]; //for server response
       bzero(server_response,sizeof(server_response)); 
       printf("Enter a message: "); 
       fgets(buffer,sizeof(buffer),stdin);
       buffer[strcspn(buffer, "\n")] = 0;  //remove trailing newline char from buffer, fgets does not remove it
       if(send(server_sd,buffer,strlen(buffer),0)<0) // error checking in sending message to server
        {
            perror("send");
            exit(-1);
        }
       if(strcmp(buffer,"BYE")==0) //if user enters BYE close the connection
        {
            break;
        }
        else{
            int bytes = recv(server_sd,server_response,sizeof(server_response),0); // receive message from server
			printf("Server response: %s\n",server_response);
        }
        
        
			
	}



	//close
	close(server_sd);
	return 0;
}
