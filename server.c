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
int startInSession(accounts ** head_ref, char* name, pthread_mutex_t *________){
	account *ptr = head_ref;
	while(ptr != NULL){

	}
}

int endInSession(accounts **head_ref, pthread_mutex_t *_______){

}
void printAccounts(){ //print account every 15 secs

}

int main(int argc, char **argv){
	int numOfAccounts =0;

	pthread_t print;
}
