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
#include <signal.h>
#include <stdbool.h>
# define SHM_SIZE 1024
# define BUFFSIZE 256

typedef struct account{
	char accountName[255];
	double balance;
	int inSession; //boolean
	struct account *next;
};

account *head_ref;

void printhandler(int sig);
void terminate(int sig);
void client_thread(int newSockfd);

boolean checkBankName(struct account** head_ref, char *name){ //check if the bankAccount already exists
  struct account *ptr = *head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			return 0;
		}
		ptr = ptr->next;
	}
	return 1;
}

int create(struct account** head_ref, char* name){ //create a new account
  //check for inSession before continuing
  if(inSession == 1){
    printf("ERROR: inSession flag is on");
    exit(0);
   }
  //check for a unique acc name
   if(checkBankName(head_ref, name) == 0){
		printf("Error: Account already exists");
		exit(0);
	}
   //creating a new node
   struct account* ptr = (struct account*)malloc(sizeof(struct account));
   struct account* last = *head_ref;
   ptr->accountName = name;
   ptr->balance =0;
   ptr->next = NULL;
   //adding to the end of the LL
   if(*head_ref==NULL){
     *head_ref = ptr;
     return 0;
   }
   while(last->next != NULL){
     last = last->next;
   }
   last->next = ptr;
  
   printf("Account created!");
   return 0;
}

int serve(accounts ** head_ref, char* name, pthread_mutex_t *mut){
	account *ptr = head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			ptr->inSession=true;
			pthread_mutex_lock(&mut);
			printf("Session service started!");
			return 0;
		}
		ptr = ptr->next;
	}
	printf("Error: Account not found");
	return 0;
}

int deposit(account* acc, double amount){
	//check to make sure acc is in service
	if(acc->inSession==0){
		printf("Error: Account not in service");
		return 0;
	}
	double bal=0.0
	bal=add(acc->balance, amount);
	acc->balance=bal;
	printf("Amount deposited!");
	return 0;
}

int withdraw(account* acc, double amount){
	if(acc->inSession==0){
		printf("Error: Account not in service");
		return 0;
	}
	//check(if withdrawal) to see if amount is larger than balance
	if(amount>(acc->balance)){
		printf("Error: Invalid withdrawal attempt");
		return 0;
	}
	double bal=0.0
	bal=(acc->balance)-amount;
	acc->balance=bal;
	printf("Amount withdrawn!");
	return 0;
}

int end(accounts *acc, pthread_mutex_t *mut){
	acc->inSession=0;
	pthread_mutex_unlock(&mut);
	printf("Session ended!");
	return 0;
}

/*void printAccounts(accounts **head_ref){ //print account every 15 secs
	printf();//print the list of accounts
}*/


int main(int argc, char **argv){
	if(argc <2){
		printf("Error: enter port number\n");
		exit(1);
	}
	char buffer[256];
	
	int numOfAccounts =0;

	signal(SIGALRM, printhandler);//retest
	alarm(15);
	signal(SIGINT, terminate);//finish

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	int port = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR: cannot bind");
	}

	if(listen(sockfd, 10)==-1){
		printf("ERROR: Listen error.\n");
		return 0;
	}
	
	while(1){
		if(listen(sockfd, 10)==-1){
			printf("ERROR: Listen error.\n");
			return 0;
		}
		else{
			int clilen = sizeof(cli_addr);
			int newSockfd = accept(sockfd,(struct sockaddr*)&cli_addr, &clilen);
			if (newSockfd < 0){
				error("ERROR on accept");
				continue;
			}
			printf("Connection to client successful.\n");
			pthread_t client;
		
			if(pthread_create(&client, 0, client_thread, &newSockfd) != 0){
				printf("ERROR: client thread could not be created\n");
				continue;
			} 
		}
	}

	return 0;
}

void client_thread(int newSockfd){
	write(newSockfd, "Connection to server successful", strlen(sendBuff));
	while(1){
		bzero(buffer,256);
		int n = read(newSockfd,buffer,255);
		if(n<0){
			error("ERROR reading from socket");
		}
		else{
			//tokenize the input
			//check what the command is
			//call the respective command
		}
		printf("Here is the message: %s\n", buffer); 
		n = write(newSockfd, "I got your message", 18); 
		if(n<0)
			error("ERROR writing to socket");
	}
}

void printhandler(int sig){
	char info[1024];
	while(ptr != NULL){
		printf("%s", ptr->accountName);
		printf("\t");
		printf("%s", ptr->balance);
		printf("\t");
		if(ptr->inSession==true)
			printf("IN SERVICE");
		ptr = ptr->next;
	}
	signal(SIGALRM, printhandler);
	alarm(15);
	return;
}

void terminate(int sig){
	printf("got sig\n");
	//stop timer
	//lock all accounts
	//disconnect all clients
	//send all clients shutdown message
	//deallocate all memory
	//close all sockets
	//join all threads
	return;
}
