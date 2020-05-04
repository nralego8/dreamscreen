// C program to demonstrate socket programming 
// as well as explicitly assigning a port number 
// on Client Side 
#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<stdlib.h> 
  
int main() 
{ 
    // Two buffer are for message communication 
    unsigned char buffer1[512], buffer2[512]; 
    struct sockaddr_in my_addr, my_addr1; 
    int client = socket(AF_INET, SOCK_DGRAM, 0); 
    if (client < 0) 
    printf("Error in client creating\n"); 
    else
        printf("Client Created\n"); 
          
    my_addr.sin_family = AF_INET; 
    my_addr.sin_addr.s_addr = INADDR_ANY; 
    my_addr.sin_port = htons(8888); 
      
    // This ip address will change according to the machine 
    my_addr.sin_addr.s_addr = inet_addr("192.168.1.161"); 
  
    // binding client with that port  
    my_addr1.sin_family = AF_INET; 
    my_addr1.sin_addr.s_addr = INADDR_ANY; 
    my_addr1.sin_port = htons(8888); 
      
    // This ip address will change according to the machine 
    my_addr1.sin_addr.s_addr = inet_addr("192.168.1.122"); 
    if (bind(client, (struct sockaddr*) &my_addr1, sizeof(struct sockaddr_in)) == 0) 
        printf("Binded Correctly\n"); 
    else
        printf("Unable to bind\n"); 
      
    socklen_t addr_size = sizeof my_addr; 
    int con = connect(client, (struct sockaddr*) &my_addr, sizeof my_addr); 
    if (con == 0) 
        printf("Client Connected\n"); 
    else
        printf("Error in Connection\n"); 
 	
	unsigned char sub[] = { 0xFC, 0x06, 0x01, 0x21, 0x01, 0x0C, 0x01, 0xFE }; 
    recv(client, buffer1, 512, 0); 
    send(client, sub, sizeof(sub), 0);  
	int i;
	for (i = 6; i < 147; i++) {
		printf("%02X ", buffer1[i]);
	}
    return 0; 
} 
