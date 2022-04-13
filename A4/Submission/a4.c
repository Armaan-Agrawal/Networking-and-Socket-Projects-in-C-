#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string.h>
#include<time.h>
#include<pthread.h>


void* connection_handler(void* client_sd_pointer){
	int client_sd = *(int*)client_sd_pointer;
	free(client_sd_pointer);
	FILE *sock_stream = fdopen(client_sd,"r+");		//open the socket as a file stream
	if(sock_stream== NULL)
	{
		perror("Can not open index.html");
		exit(-1);
	}
	
	char buffer[256];
	char requestMessage[100];
	char temp[256];
	char delim[] = " ";

	//to get localtime for HTTP Header Date
	time_t rawtime;
	time_t t;   
    time(&t);
	struct tm *info;
	char TimeBuffer[80];

	time( &rawtime );

	



	fgets(buffer,sizeof(buffer),sock_stream);	//read the first line of the request
	strncpy(requestMessage,buffer,strlen(buffer)-1);
	strcpy(temp,buffer);
	char* ptr = strtok(temp,delim);
	ptr = strtok(NULL, delim); //parsing request message to get the requested file
	
	char fileType[256];
	strcpy(fileType,ptr);
	char* ftype = strtok(fileType,".");
	ftype = strtok(NULL, "."); //parsing requested file to get file type (html,css..)
	

	char content_type[256];

	if(strcmp(ftype,"html")==0){
		strcpy(content_type, "text/html");
	}
	else if(strcmp(ftype,"css")==0){
		strcpy(content_type, "text/css");
	}
	else if(strcmp(ftype,"js")==0){
		strcpy(content_type, "application/javascript");
	}
	else if(strcmp(ftype,"png")==0){
		strcpy(content_type, "image/png");
	}

	char FullFileName[256] = "../files"; //..files/index.html
	strcat(FullFileName,ptr); 
	
	fgets(buffer,sizeof(buffer),sock_stream);	//read the second line of the request
	char host[256];
	strcpy(host,buffer);
	char* ptr2 = strtok(host,delim);
	ptr2 = strtok(NULL, delim); //to get host IP, ie. 127.0.0.1
	
	char hostIP[20];
	strcpy(hostIP,ptr2);
	hostIP[strcspn(hostIP, "\r\n")] = 0;

	
	// printf("%s \n",buffer);
	


	char currentTime[256];
	strcpy(currentTime,ctime(&t));
	currentTime[strcspn(currentTime, "\n")] = 0;
	FILE *fptr = fopen(FullFileName,"r");		//open requested file
	if(fptr == NULL) //serving error message in case file not present in disk
	{	
		
		char errorMessage[] = "<html><head><title>My Web page</title></head><body><h1>Error. File Not Found ;(</h1></body></html>";
		fprintf(sock_stream,"HTTP/1.0 404 Not Found\nServer: SimpleHTTPServer\nDate: %s\nContent-type: text/html\nContent-Length: %lu\r\n\n",currentTime,sizeof(errorMessage)); 
		fwrite(errorMessage,sizeof(errorMessage),1,sock_stream);
		char perrorMessage[256] = "Can not open ";
		strcat(perrorMessage,FullFileName); 
		perror(perrorMessage);
		exit(-1);
	}

	info = localtime( &rawtime );
	strftime(TimeBuffer,80,"%d/%b/%Y %I:%M:%S", info);

	requestMessage[strcspn(requestMessage, "\r")] = 0;
	printf("%s [%s] \"%s\" 200\n",hostIP,TimeBuffer,requestMessage);
	


	fseek(fptr,0,SEEK_END);						//move the file pointer to the end of the file
	int file_size = ftell(fptr);				//read the position of the filepointer which will be equal to the size of the file
	rewind(fptr);								//rewind the file pointer back to the start of the file
	//make a very simple http header
	

	fprintf(sock_stream,"HTTP/1.0 200 OK\nServer: SimpleHTTPServer\nDate: %s\nContent-type: %s\nContent-Length: %d\r\n\n",currentTime,content_type,file_size); 
	
	char *file_contents = malloc(file_size); 	//create a buffer to store index.html
	fread(file_contents,file_size,1,fptr);		//read index.html from disc and store in file_contents buffer
	fwrite(file_contents,file_size,1,sock_stream);	//write/send the file_content to the client socket

	fflush(sock_stream);	//flush the file stream associated with the socket
	fclose(sock_stream);	
	close(client_sd);		//close the client/secondary socket
	free(file_contents); 	//remove the dynamic memory
	return NULL;
}



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

	//4. accept clients
	while(1)
	{
		int client_sd = accept(server_sd,NULL,NULL);
		
		if(client_sd<1)
		{
			perror("Accept Error:");
			return -1;
		}
		else
		{
			// implementing threading to handle connection, serve files and eventually close connection
			int* client_pointer = malloc(sizeof(int));
			*client_pointer = client_sd;
			pthread_t thread;
			pthread_create(&thread,NULL,connection_handler,client_pointer);
			
		}
	}


	close(server_sd);	//close the master/server socket
	return 0;									
	// 6. close
}