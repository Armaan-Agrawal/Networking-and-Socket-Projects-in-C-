#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string.h>

int main()
{

	//1. socket
	int server_sd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sd<0)
	{
		perror("Socket Failed: ");
		return -1;
	}

	//2. bind
	struct sockaddr_in server_address;	//structure to save IP address and port
	memset(&server_address,0,sizeof(server_address)); //Initialize/fill the server_address to 0
	server_address.sin_family = AF_INET;	//address family
	server_address.sin_port = htons(80);	//port
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); //htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");

	setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)); //&(int){1},sizeof(int)


	if(bind(server_sd,(struct sockaddr *)&server_address,sizeof(server_address))<0) 
	{
		perror("Bind failed..:");
		return -1;
	}

	//3. listen
	if(listen(server_sd,5)<0)
	{
		perror("Listen Error:");
		return -1;
	}


	struct sockaddr_in client_address;

	//4. accept
	while(1)
	{
		int client_sd = accept(server_sd,NULL,NULL);
		//int client_len=sizeof(client_address);
		//int client_sd = accept(server_sd,(struct sockaddr*)&client_address,(socklen_t *)(&client_len));
		if(client_sd<1)
		{
			perror("Accept Error:");
			return -1;
		}
		else
		{
			
			FILE *sock_stream = fdopen(client_sd,"r+");		//open the socket as a file stream
			if(sock_stream== NULL)
			{
				perror("Can not opne index.html");
				exit(-1);
			}
			
			char buffer[256];
			fgets(buffer,sizeof(buffer),sock_stream);	//read the first line of the request
			printf("%s \n",buffer);
			fgets(buffer,sizeof(buffer),sock_stream);	//read the second line of the request
			printf("%s \n",buffer);

			FILE *fptr = fopen("index.html","r");		//open index.html
			if(fptr == NULL)
			{
				perror("Can not open index.html");
				exit(-1);
			}
			fseek(fptr,0,SEEK_END);						//move the file pointer to the end of the file
			int file_size = ftell(fptr);				//read the position of the filepointer which will be equal to the size of the file
			rewind(fptr);								//rewind the file pointer back to the start of the file
			//make a very simple http header
			fprintf(sock_stream,"HTTP/1.0 200 OK\nServer: SimpleHTTPServer\nContent-type: text/html\nContent-Length: %d\r\n\n",file_size); 
			
			char *file_contents = malloc(file_size); 	//create a buffer to store index.html
			fread(file_contents,file_size,1,fptr);		//read index.html from disc and store in file_contents buffer
			fwrite(file_contents,file_size,1,sock_stream);	//write/send the file_content to the client socket

			fflush(sock_stream);	//flush the file stream associated with the socket
			fclose(sock_stream);	
			close(client_sd);		//close the client/secondary socket
			free(file_contents); 	//remove the dynamic memory
		}

	}	

	close(server_sd);										//close the master/server socket
	//6. close
}