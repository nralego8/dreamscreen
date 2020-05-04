// C program to demonstrate  
// socket programming in finding ip address  
// and port number of connected client 
// on Server Side 
#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<sys/un.h> 
#include<string.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h> 
  
int main() 
{ 
    // Two buffers for message communication 
    char buffer1[256], buffer2[256]; 
    int server = socket(AF_INET, SOCK_DGRAM, 0); 
    if (server < 0) 
        printf("Error in server creating\n"); 
    else
        printf("Server Created\n"); 
          
    struct sockaddr_in my_addr, peer_addr; 
    my_addr.sin_family = AF_INET; 
    my_addr.sin_addr.s_addr = INADDR_ANY; 
      
    // This ip address will change according to the machine 
    my_addr.sin_addr.s_addr = inet_addr("192.168.1.122"); 
      
    my_addr.sin_port = htons(8888); 
  
    if (bind(server, (struct sockaddr*) &my_addr, sizeof(my_addr)) == 0) 
        printf("Binded Correctly\n"); 
    else
        printf("Unable to bind\n"); 
          
    if (listen(server, 3) == 0) 
        printf("Listening ...\n"); 
    else
        printf("Unable to listen\n"); 
      
    socklen_t addr_size; 
    addr_size = sizeof(struct sockaddr_in); 
      
    // Ip character array will store the ip address of client 
    char *ip; 
      
    // while loop is iterated infinitely to  
    // accept infinite connection one by one 
    while (1) 
    { 
        int acc = accept(server, (struct sockaddr*) &peer_addr, &addr_size); 
	//unsigned char sub[] = { 0xFC, 0x06, 0x01, 0x21, 0x01, 0x0C, 0x01, 0xFE }; 
    	
	//send(server, sub, sizeof(sub), 0);  
    recv(server, buffer1, 256, 0);
	//printf("%s\n", buffer1);
	int i;
	for (i = 6; i < 147; i++) {
		printf("%02X ", buffer1[i]);
	}
	printf("\n");
}  
    return 0; 
} 
