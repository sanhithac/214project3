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
		printf("Error sowwy no can do");
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
	int numOfAccounts =0;

	pthread_t print;
}
