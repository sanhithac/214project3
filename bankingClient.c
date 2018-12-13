#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>	
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <signal.h>


static int commandControl; 
char startedsession[100];  
int quiting =-1;	      
int sockd;
struct sockaddr_in serv_addr, cli_addr;

char buffer[256];
char sendToServerBuff[256];

error(char*void msg);
void sig_handler(int signo);
int checkConnection(int sd);
void *readToServer(void *fd);
void *readFromServer(void *fd);

//done
error(char*void msg){
	perror(msg);
	exit(1);
}
//done
void sig_handler(int signo){
	char quitbuffer[256] = "quit ";
	if(signo==SIGINT) {
		if(quiting==0){
			printf("\nExititing client abruptly.. finishing started session\n");
			write(sockd,strcat(quitbuffer,startedsession),256);
			exit(0);
		}
		else{
			printf("\nExiting client\n");
			exit(0);
		}
	}
}

int checkConnection(int sd) 
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
//done
void *readToServer(void *fd){ //sends to server
	//send
	int sockfd = *(int*)fd;
	while(1){
		printf("Please enter a command followed by a value (if applicable):");
		char command[256];

		memset(command, 0, 256);
		memset(sendToServerBuff, 0, strlen(sendToServerBuff));

		char op[255];
		char arg[255];
		char err[255];
		fgets(command,256, stdin); //read command from user
		if((strlen(command) > 0) && (command[strlen(command)-1] == '\n')){
			command[strlen(command)-1] = '\0';
		}
		int commandAssigned = sscanf(command, "%s %s %s", op, arg, err);
		printf("%s\n", command);
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
		if((send(sockfd,sendToServerBuff, strlen(sendToServerBuff),0))== -1) {
            printf("ERROR: Could not send message.\n");
            close(sockfd);
            exit(1);
        }
        sleep(2);
	}
	return NULL;
}
//done
void *readFromServer(void *fd){ //read from server
	//recv
	int sockfd = *(int*)fd;
	while(1){
		
		int num = recv(sockfd, sendToServerBuff, sizeof(sendToServerBuff),0);
		while(num > 0){
		  printf("%s\n", buffer);
		  memset(buffer, 0, strlen(buffer));
		}
		if(strcmp(sendToServerBuff, "end")==0){
			printf("Client closing.\n");
			exit(0);
		}
		/*	if(sendToServerBuff[0] == NULL){
			printf("%s\n", sendToServerBuff);
			} */
	}
	return NULL;
}
//done
int main(int argc, char** argv){
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

	struct hostent*server_ip = gethostbyname(argv[1]);
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	//	serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
	bcopy((char*) server_ip->h_addr, (char*)&serv_addr.sin_addr, server_ip ->h_length);

	//attempt connecting to the server
	int try = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

			  /*	while(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) <0){
		printf("Establishing connection with server...\n");
		sleep(3);
	}
			  */
			  if(try <0){
			    printf("Connectin to socket fail");
			  }
	//first read to insure connection
	int n;
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
	signal(SIGINT, sig_handler);
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
