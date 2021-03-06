#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#define PORT 9033

typedef struct Account{
  char* accountName;
	double balance;
	int inSession; //boolean
	struct Account *next;
}account;

int newSock;
account *head_ref;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;

//pthread_mutex_t mut;

int checkBankName(account* head_ref, char *name);
void quit(account* acc);
void query(account* acc);
int end(account *acc, pthread_mutex_t *mut);
account *create(account* head_ref, char* name);
account *serve(account * head_ref, char* name, pthread_mutex_t *mut);
int deposit(account* acc, double amount);
int withdraw(account* acc, double amount);
void printhandler(int sig);
void *client_thread(void* newSockfd);
void terminate(int sig);

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

void quit(account* acc){
  acc->inSession =0;
  write(newSock, "You have exited the bank.\n",sizeof(buffer));
  printf("Client has now disconnected from the server\n");
}

void query(account* acc){
  char message[600];
  sprintf(message,"Your account balance is $%.2f.\n\n", account->balance);
  write(newSock, message, sizeof(message)-1);
}

int end(account *acc, pthread_mutex_t *mut){
	acc->inSession=0;
	write(newSock,"Session ended!\n",strlen(buffer));
	//pthread_mutex_unlock(&mut);
	return 0;
}

account *create(account* head_ref, char* name){ //create a new account
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
     return ptr;
   }
   while(last->next != NULL){
     last = last->next;
   }
   last->next = ptr;
  
   printf("Account created!");
   return ptr;
}

account *serve(account * head_ref, char* name, pthread_mutex_t *mut){
	account *ptr = head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			ptr->inSession=1;
			//	pthread_mutex_lock(&mut);
			printf("Session service started!");
			return ptr;
		}
		ptr = ptr->next;
	}
	printf("Error: Account not found");
	return NULL;
}

int deposit(account* acc, double amount){
	//check to make sure acc is in service
	if(acc->inSession==0){
		printf("Error: Account not in service");
		return 0;
	}
	double bal=0.0;
	bal=acc->balance+ amount;
	acc->balance=bal;
	printf("Amount deposited!");
	return 0;
}

int withdraw(account* acc, double amount){
	if(acc->inSession==0){
		printf("Error: Account not in service");
		return 0;
	}
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

void printhandler(int sig){
	account  *ptr =head_ref;
	while(ptr != NULL){
	  printf("Account Name is equal to:%s\t Account Balance is equal to:  %lf\t",ptr->accountName,ptr->balance);
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
	account *acc;
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
			        acc=create(head_ref, name);
			      }
			      else{
			  	     write(*newSock, "ERROR: Enter name", strlen("ERROR: Enter name"));
			      }
			    }else{
				    write(*newSock, "ERROR: cannot create account while in session", strlen("ERROR: cannot create account while in session"));
			    }
				  //SERVE
			  }else if(strcmp(command, "serve")==0){
			    if(inSessionOn==0){
			      if(token!=NULL){
			        char *name=strtok(NULL, s);
			        acc=serve(head_ref, name, mut);
				 inSessionOn=1;
			      }else{
			  	     write(*newSock, "ERROR: Enter name", strlen("ERROR: Enter name"));
			      }
			    }else{
				    write(*newSock, "ERROR: cannot start another service while in session", strlen("ERROR: cannot start another service while in session"));
			    }
				  //DEPOSIT
			  }else if(strcmp(command, "deposit")==0){
			    if(inSessionOn==1){
			   	 if(token!=NULL){
			     		 double amt=strtok(NULL, s);
					 deposit(acc, amt);
				 }else{
			  	     write(*newSock, "ERROR: Enter amount", strlen("ERROR: Enter amount"));
				 }
			    }else{
				    write(*newSock, "ERROR: cannot deposit before starting a service", strlen("ERROR: cannot deposit before starting a service"));
			    }
				  //WITHDRAW
			  }else if(strcmp(command, "withdraw")==0){
			    if(inSessionOn==1){
			   	 if(token!=NULL){
			     		 double amt=strtok(NULL, s);
					 withdraw(acc, amt);
				 }else{
			  	     write(*newSock, "ERROR: Enter amount", strlen("ERROR: Enter amount"));
				 }
			    }else{
				    write(*newSock, "ERROR: cannot withdraw before starting a service", strlen("ERROR: cannot withdraw before starting a service"));
			    }
				  //QUERY
			  }else if(strcmp(command, "query")==0){
				  if(inSessionOn==1){
					  query(acc)
				  }else{
				    write(*newSock, "ERROR: cannot query before starting a service", strlen("ERROR: cannot query before starting a service"));
				  }
				  //END
			  }else if(strcmp(command, "end")==0){
				  if(inSessionOn==1){
					 end(acc, mut);
					  inSessionOn=0;
				  }else{
				    write(*newSock, "ERROR: cannot end session before starting a service", strlen("ERROR: cannot end session before starting a service"));
				  }
				  //QUIT
			  }else if(strcmp(command, "quit")==0){
				  quit(acc);
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
	char buffer[256];
	int numOfAccounts =0;
	
	signal(SIGALRM, printhandler);//retest
 	alarm(15);
 	signal(SIGINT, terminate);//finish
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/*if(sockfd < 0){
		error("Error opening Socket");
	} */
	
	//bzero((char *)&serv_addr, sizeof(serv_addr)); //erases memory at this addr
	int port = atoi(argv[1]);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	
	//int k = gethostname(argv[1]);
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}
	if(listen(sockfd, 10)==-1){
		printf("ERROR: Listen error.\n");
		return 0;
		//change the 2 later to numOfClients that you want
	}
	
	while(1){
		if(listen(sockfd, 10)==-1){
			printf("ERROR: Listen error.\n");
			return 0;
			//change the 2 later to numOfClients that you want
		}
		else{
			int clilen = sizeof(cli_addr);
			int newSockfd = accept(sockfd,(struct sockaddr*)&cli_addr, &clilen);
			if (newSockfd < 0){
				error("ERROR on accept");
				continue;
			}
			printf("Connection to client successful.\n");
			//create new thread for client
			pthread_t client;
		
			if(pthread_create(&client, 0, client_thread,(void *)&newSockfd) != 0){
				printf("ERROR: client thread could not be created\n");
				continue;
			}		
		}
	return 0;
	}
}
void terminate(int sig){
 	//stop timer
 	//lock all accounts
 	//disconnect all clients
 	//send all clients shutdown message
 	//deallocate all memory
 	//close all sockets
 	//join all threads
 	return;
}
