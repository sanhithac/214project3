#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 9033 

void error(char* msg){
	perror();
	exit(1);
}

int isConnected(int something){

}

int main(int argc, char** argv){
	char buffer[256];

	if(argc != 2){
		error("Error. Invalid arguments");
		exit(1);
	}
	
	//bzero((char *)&serv_addr, sizeof(serv_addr)); //erases memory at this addr
	int port = atoi(argv[1]);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error opening Socket");
	}

	int server = gethostname(argv[0]);
	if(server == NULL){
		printf("Error, no such host");
		exit(0);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(port);
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0){
		//printf("Established connection");
		error("ERROR connecting");
	}
	else{
		printf("Connection Complete");
	} 
	pritnf("Please eenter a message:");
	bzero(buffer,256)
	fgets(buffer,255,stdin);
	int n =write(sockfd, buffer,strlen(buffer));
	if (n<0)
		error("Error on writing to socket");
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n<0)
		error("ERROR  reading from packet");

	return 0;
}
