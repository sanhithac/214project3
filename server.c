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
}

int startInSession(accounts ** head_ref, char* name, pthread_mutex_t *_____){
	account *ptr = head_ref;
	while(ptr != NULL){

	}
}

int endInSession(accounts **head_ref, pthread_mutex_t *_______){

}
void printAccounts(accounts **head_ref){ //print account every 15 secs
	printf();//print the list of accounts
}


int main(int argc, char **argv){
	int numOfAccounts =0;

	pthread_t print;
}
