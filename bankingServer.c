//server
typedef struct account{
	char accountName[255];
	double balance;
	int inSession; //boolean
	struct account *next;
};

boolean checkBankName(struct accounts** head_ref, char *name){ //check if the bankAccount already exists
	account *ptr = head_ref;
	while(ptr != NULL){
		if(ptr->accountName == name){
			return 0;
		}
		ptr = ptr->next;
	}
	return 1;
}

int create(account* acc, char* name){
	if(checkBankName() == 0){
		printf("Error: Account already exists");
	}
	//create new Node
	account->accountName = name;
	account->balance = 0;
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
	if(acc->inSession==false){
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
	if(acc->inSession==false){
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
	acc->inSession=false;
	pthread_mutex_unlock(&mut);
	printf("Session ended!");
	return 0;
}

void printAccounts(accounts **head_ref){ //print account every 15 secs
	printf();//print the list of accounts
}


int main(int argc, char **argv){
	if(argc <2){
		printf("Error");
		exit(1);
	}
	char buffer[256];
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error opening Socket");
	} 
	bzero((char *)&serv_addr, sizeof(serv_addr)); //erases memory at this addr
	int port = atoi(argv[1]);
	serv_addr.sin_family = AF_NET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	//int k = gethostname(argv[1]);
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}
	listen(sockfd, 2); //change the 2 later to numOfClients that you want
	
	int clilen = sizeof(cli_addr);
	int newSockfd = accept(sockfd,(struct sockaddr*)&cli_addr, &clilen);
	if (newSockfd < 0){
		error("ERROR on accept");
	}

	bzero(buffer,256);
	int n = read(newSockfd,buffer,255);
	if(n<0)
		error("ERROR reading from socket");
	printf("Here is the message: %s\n", buffer); 
	n = write(newSockfd, "I got your message", 18); 
	if(n<0)
		error("ERROR writing to socket");
	return 0;
	
	//int numOfAccounts =0;
	//signal(SIGALARM, handler);
	//pthread_t print;
}

void handler(int sig){
	accounts *ptr=head_ref;
	while(ptr != NULL){
		printf(ptr->accountName+"\t"+ptr->balance+"\t");
		if(ptr->inSession==true)
			printf("IN SERVICE");
		ptr = ptr->next;
	}
	alarm(15);
	signal(SIGALRM, handler);
}
