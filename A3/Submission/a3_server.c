//server

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
	struct sockaddr_in server_addr, client_addr;

	bzero(&server_addr,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY, INADDR_LOOP

	//bind
	if(bind(server_sd, (struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
		perror("bind failed");
		exit(-1);
	}
	//listen
	if(listen(server_sd,5)<0)
	{
		perror("listen failed");
		close(server_sd);
		exit(-1);
	}
	

	while(1)
	{
		//accept
        unsigned int client_len, n;
        client_len = sizeof(client_addr); 

		int client_sd = accept(server_sd,(struct sockaddr *) &client_addr,&client_len);
		
		int pid = fork(); //fork a child process


		if(pid ==0) // to ensure child process
		 {  
            char client_ip[INET_ADDRSTRLEN]; 
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN); //using methods from  #include <arpa/inet.h> to store client IP address
			printf("[%s:%d] Connected \n", client_ip, ntohs(client_addr.sin_port));

            
		 	
			while(1)
			{   
                char client_message[256]; //to receive client's message 
				bzero(client_message,sizeof(client_message));
				int bytes = recv(client_sd,client_message,sizeof(client_message),0); // receive message from client
				if(bytes==0 | bytes > 100 | strcmp(client_message,"BYE!")==0)   // to ensure that client has closed the connection
				{
					printf("[%s:%d] Disconnected\n", client_ip, ntohs(client_addr.sin_port));
					close(client_sd);
					break;
				}
                printf("[%s:%d] Received message: %s\n", client_ip, ntohs(client_addr.sin_port), client_message);
                if (strcspn(client_message, "BYE!") != 0) {
				    
                    send(client_sd,client_message,sizeof(client_message),0); //send message back to client
					printf("[%s:%d] Send message: %s\n", client_ip, ntohs(client_addr.sin_port), client_message);
                }

			}
		 }
	}


	//recv/send

	//close
	close(server_sd);
	return 0;
}
