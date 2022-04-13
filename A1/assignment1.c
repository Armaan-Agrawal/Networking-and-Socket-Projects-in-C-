#include<stdio.h>  
#include<stdlib.h>
#include<string.h>


int main(int argc, char *argv[] ){  
    
    if(argc < 2){  // error message for missing commandline arguement; arguement length 2 (name of program and name of file)
        printf("Error. No argument passed through command line.\n");
        exit(-1);  
    }  
    else{  

        typedef struct      //UDP packet structure to read data
        {
            unsigned short int src_port;
            unsigned short int des_port;
            unsigned short int length;
            unsigned short int checksum;
        } UDP;


        
        FILE* fptr;         
        char ch;

       
        fptr = fopen(argv[1], "rb");  // Opening file in binary read mode
        
        if (fptr == NULL) { // error handling for improper file opening 
            perror("Error. File could not open because: ");
            return(1);
        }else{
            int filesize;   
            fseek(fptr,0,SEEK_END);
            filesize = ftell(fptr);  //counting total number of bytes in file
            rewind(fptr);   //moving pointer back to beginning of file to start file reading
     

            fseek(fptr,24,SEEK_SET);        //skipping 24 bytes of global header as instructed 
            // while(!feof(fptr)){            tried this but did not work, printed additional package
            while(ftell(fptr) < filesize){
                    UDP p1;
                    fseek(fptr,50,SEEK_CUR);        //skipping first few headers in order to read udp header
                    fread(&p1,sizeof(p1),1,fptr);   //reading udp header using udp struct
                    printf("\n----------------------\n");
                    printf("Src Port: %hu\nDes Port: %hu\nUDP Packet Length: %hu\nChecksum: %#x\n", ntohs(p1.src_port), ntohs(p1.des_port), ntohs(p1.length), ntohs(p1.checksum)); //using ntohs to convert from network byte to host byte
                    int data_size = ntohs(p1.length) - 8; // data size  = length - size of headers
                    

                    for(int i =0; i<data_size; i++) { // getting data character by character
                        int c;
                        c  = fgetc(fptr);
                        if (c> 127 || c < 33 || c == ' ') printf("."); //if character's ascii is not for a symbol, alphabet or number print "."
                        else {
                        printf("%c", c);
                        }
                    }
                    printf("\n");

                }
                printf("\n");
            }
            fclose(fptr);
        }

        
        return(0);
    }  
    
