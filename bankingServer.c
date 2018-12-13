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

typedef struct Account{
  char* accountName;
	double balance;
	int inSession; //boolean
	struct Account *next;
}account;

account *head_ref;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
//create mutex

void printhandler(int sig);
void terminate(int sig);
void *client_thread(void* newSockfd);

int checkBankName(account* head_ref, char *name){ //check if the bankAccount already exists
   account *ptr = head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			return 0;
		}
		ptr = ptr->next;
	}
	return 1;
}

int create(account* head_ref, char* name){ //create a new account
  //check for inSession before continuing
 /* if(inSessionOn == 1){
    printf("ERROR: inSession flag is on");
    exit(0);
   }
*/
  //check for a unique acc name
   if(checkBankName(head_ref, name) == 0){
		printf("Error: Account already exists");
		exit(0);
	}
   //creating a new node
   account* ptr = (account*)malloc(sizeof(account));
   account* last = head_ref;
   ptr->accountName = malloc(strlen(name)+1);
   strcpy(ptr->accountName,name);
   ptr->balance =0;
   ptr->next = NULL;
   //adding to the end of the LL
   if(head_ref==NULL){  
     head_ref = ptr;
     return 0;
   }
   while(last->next != NULL){
     last = last->next;
   }
   last->next = ptr;
  
   printf("Account created!");
   return 0;
}

int serve(account * head_ref, char* name, pthread_mutex_t *mut){
	account *ptr = head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			ptr->inSession=1;
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
	double bal=0.0;
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
	double bal=0.0;
	bal=(acc->balance)-amount;
	acc->balance=bal;
	printf("Amount withdrawn!");
	return 0;
}

int end(account *acc, pthread_mutex_t *mut){
	acc->inSession=0;
	pthread_mutex_unlock(&mut);
	printf("Session ended!");
	return 0;
}

void printhandler(int sig){
	account  *ptr =head_ref;
	while(ptr != NULL){
		printf(ptr->accountName+"\t"+ptr->balance+"\t");
		if(ptr->inSession==1)
			printf("IN SERVICE");
		ptr = ptr->next;
	}
	alarm(15);
	signal(SIGALRM, printhandler);
}

void *client_thread(void* newSockfd){
        int *newSock=(int *)newSockfd;
	char buffer[256];
	int inSessionOn=0;//boolean
	write(*newSock, "Connection to server successful", strlen("Connection to server successful"));
	while(1){
		bzero(buffer,256);
		int n = read(*newSock,buffer,255);
		if(n<0){
			error("ERROR reading from socket");
		}
			else{
	        
			  const char s[2]=" ";
			  char *command=strtok(buffer, s);
		     		//CREATE
			  if(strcmp(command, "create")==0){
			    if(inSessionOn==0){
			      if(token!=NULL){
			        char *name=strtok(NULL, s);
			        create(head_ref, name);//return account
			      }
			      else{
			  	     printf("ERROR: enter name");//make these writes
			      }
			    }else{
				    printf("ERROR: cannot create account while in session");
			    }
				  //SERVE
			  }else if(strcmp(command, "serve")==0){
			    if(inSessionOn==0){
			      if(token!=NULL){
			        char *name=strtok(NULL, s);
			        //serve(name, mut)//return account
				 inSessionOn=1;
			      }else{
			  	       printf("ERROR: enter name");
			      }
			    }else{
				    printf("ERROR: cannot start another service while in session");
			    }
				  //DEPOSIT
			  }else if(strcmp(command, "deposit")==0){
			    if(inSessionOn==1){
			   	 if(token!=NULL){
			     		 double amt=strtok(NULL, s);
					 //deposit(account, amt);
				 }else{
					 printf("ERROR: enter amountt");
				 }
			    }else{
				    printf("ERROR: cannot deposit before starting a service");
			    }
				  //WITHDRAW
			  }else if(strcmp(command, "withdraw")==0){
			    if(inSessionOn==1){
			   	 if(token!=NULL){
			     		 double amt=strtok(NULL, s);
					 //withdraw(account, amt);
				 }else{
					 printf("ERROR: enter amount");
				 }
			    }else{
				    printf("ERROR: cannot withdraw before starting a service");
			    }
				  //QUERY
			  }else if(strcmp(command, "query")==0){
				  if(inSessionOn==1){
					  //print balance of account
				  }else{
					  printf("ERROR: cannot query before starting a service");
				  }
				  //END
			  }else if(strcmp(command, "end")==0){
				  if(inSessionOn==1){
					 // end(acc, mut);
					  inSessionOn=0;
				  }else{
					  printf("ERROR: cannot end session before starting a service");
					  
				  }
				  //QUIT
			  }else if(strcmp(command, "quit")==0){
			  }else{
			    printf("ERROR: command not valid");
			  }
		}
		/*	printf("Here is the message: %s\n", buffer); 
		n = write(newSockfd, "I got your message", 18); 
		if(n<0)
		error("ERROR writing to socket");*/
	}
	return;

}

int main(int argc, char **argv){
	if(argc <2){
		printf("Error: enter port number\n");
		exit(1);
	}
	
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
		
			if(pthread_create(&client, 0, client_thread,(void *)&newSockfd) != 0){
				printf("ERROR: client thread could not be created\n");
				continue;
			} 
		}
	}

	return 0;
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
