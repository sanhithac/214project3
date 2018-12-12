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
	int port = atoi(argv[2]);

	if(argc != 3){
		error("Error. Invalid arguments");
		exit(1);
	}


	memset(buffer, '0', sizeof(buffer));
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error opening Socket");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(argv[2]);


	//attempt connecting to the server
	while(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) <0){
		printf("Establishing connection with server...\n");
		sleep(3);
	}

	//first read to insure connection
	if((n=read(sockfd,buffer,sizeof(buffer)-1)) > 0){
		buffer[n] =0;
		if(fputs(buffer, stdout) == EOF){
			printf("ERROR: Fputs error.\n");
		}
	}
	if(n<0){
		printf("ERROR: Read error.\n");
	}
	pthread_t toServer;
	pthread_t fromServer;
	signal(SIGINT, sigint_handler);
	if(pthread_create(&toServer, NULL, &readToServer, &sockfd) != 0){
		printf("ERROR: Failure launching command input thread.\n");
		exit(1);
	}
	if(pthread_create(&fromServer, NULL, &readFromServer, &sockfd) != 0){
		printf("ERROR: Failure launching response output thread.\n");
		exit(1);
	}
	
	pthread_join(toServer, NULL);
	pthread_join(fromServer, NULL);
	printf("Client end.\n");
	return 0;
}
