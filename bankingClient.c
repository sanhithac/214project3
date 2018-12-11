//client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <signal.h>

#define BUFFSIZE 256

static int commandControl; //switch that controls thread synchronization
char startedsession[100];  //hold the name of the session started. If in case signint occurs use this to finish the session
int finished =-1;	       //flag -1 -> session not started 0 -> session started not finished 1->session finished
int sockd;
int serv_addr, cli_addr;

void error(char* msg){
	perror(msg);
	exit(1);
}
void sig_handler(int signo){
	char finbuffer[BUFFSIZE] = "finish ";
	if(signo==SIGINT) {
		if(finished==0){
			printf("\nExititing client abruptly.. finishing started session\n");
			write(sockd,strcat(finbuffer,startedsession),BUFFSIZE);
			exit(0);
		}
		else{
			printf("\nExiting client\n");
			exit(0);
		}
	}
}

int checkConnection(int sd) //spammed periodically to check whether the server is still running, otherwise terminates client.
{
	int len = sizeof(cli_addr);
	int retval = getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &len);

	if (retval != 0) {
		/* there was a problem getting the error code */
		exit(0);
		return 0;
	}
	else if (len != 0) {
		/* socket has a non zero error status */
		exit(0);
		return 0;
	}
	else 
		return 1;
}

void *readToServer(void *fd){
	int sd = *(int*)fd;
	char clibuff[BUFFSIZE]="";
	char *token="";
	while(1){ //while exit condition is not provided by the user
		bzero(clibuff, BUFFSIZE); //clearing out the buffer to recieve the command
		int size_input=read(0, clibuff, BUFFSIZE);
		if(!checkConnection(sd)) 
			exit(0);
		//update information if command is start
		if(strncmp(clibuff,"create",6) == 0){
			strncpy(startedsession,clibuff+6,size_input-6);		// store the name of session
			startedsession[size_input-5]=='\0';
			finished=0;											//session started, not finished
			sockd=sd;											//update global variable 
			}
		else if(strncmp(clibuff,"finish",6)==0)					//update finished flag to 1
			finished=1;
			
		write(sd, clibuff, size_input);
		sleep(2);
		if(!checkConnection(sd)) exit(0);
		token=strtok(clibuff, "0");
	}	
	commandControl=1; //thread sync
	exit(0);
	return 0;
}

void *readFromServer(void *fd){ //read from server
	char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int network_socket_fd = *(int *)fd;
    int status;
    while(1){
    	memset(buffer, 0, sizeof(buffer));
    	
    }
    while((status = read(network_socket_fd, buffer, sizeof(buffer)) ) > 0 ){
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    printf("The Bank server has closed connection");
    close(network_socket_fd);
    free(fd);
    return 0;
}


int main(int argc, char** argv){
	char buffer[256];

	if(argc != 3){
		error("Error. Invalid arguments");
		exit(1);
	}
	
	//bzero((char *)&serv_addr, sizeof(serv_addr)); //erases memory at this addr
	int port = atoi(argv[2]);

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
	if(connect(sockfd, (struct sockaddr *)&serv_addr, &len) <0){
		//printf("Established connection");
		error("ERROR connecting");
	}
	else{
		printf("Connection Complete");
	} 
	printf("Please eenter a message:");
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
