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
	//send
	char sendToServerBuff[256];
	int sockfd = *(int*)fd;
	while(1){
		printf("Please enter a command followed by a value (if applicable):");
		char command[256];

		memset(command, 0, 256);
		memset(sendToServerBuff, 0, strlen(sendToServerBuff));

		char op[255];
		char arg[255];
		fgets(command,256, stdin); //read command from user
		if((strlen(command) > 0) && (command[strlen(command)-1] == '\n')){
			command[strlen(command)-1] = '\0';
		}
		int commandAssigned = sscanf(command, "%s %s %s", op, arg, err);
		
		//If there are two arguments then they will be concatenated
		if(commandAssigned == 2){
			strcat(op, " ");
			strcat(op, arg);
		//If there are more than two arguments then the command is invalid.			
		} else if(commandAssigned > 2){
			printf("ERROR: Command not recognized.\n");
			continue;
		} 
		strcpy(sendToServerBuff, op);
		if((send(sockfd,sendBuff, strlen(sendBuff),0))== -1) {
            printf("ERROR: Could not send message.\n");
            close(sockfd);
            exit(1);
        }
        sleep(2);
	}
	return NULL;
}

void *readFromServer(void *fd){ //read from server
	//recv
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
