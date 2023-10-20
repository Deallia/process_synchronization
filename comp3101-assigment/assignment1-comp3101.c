#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>

sem_t *semaphore;
int *balance;
FILE *fpt;

void deposit(int amt){
    sem_wait(semaphore);
    *balance += amt;
    pid_t pid_d = getpid();
    pid_t ppid_d = getppid();

    fpt=fopen("account.dat","w");
	fprintf(fpt, "Balance: %d", *balance);
    fclose(fpt);

    fpt=fopen("transaction.dat","a");
	fprintf(fpt, "Process ID: %d, Parent Process ID: %d, Transaction type: DEPOSIT, Prev. balance: %d, Current balance: %d\n", pid_d, ppid_d, *balance - amt, *balance); 
    printf("\nNew balance after deposit: %d\n", *balance);
    fclose(fpt);
    sem_post(semaphore);
    
    
    
}

void withdraw(int amt){
    sem_wait(semaphore);
    *balance -= amt;
    pid_t pid_w = getpid();
    pid_t ppid_w = getppid();

    fpt=fopen("account.dat","w");
	fprintf(fpt, "Balance: %d", *balance);
    fclose(fpt);

    fpt=fopen("transaction.dat","a");
	fprintf(fpt, "Process ID: %d, Parent Process ID: %d, Transaction type: WITHDRAWAL, Prev. balance: %d, Current balance: %d\n", pid_w, ppid_w, *balance + amt, *balance); 
    printf("\nNew balance after withdrawal: %d\n", *balance);
    fclose(fpt);
    sem_post(semaphore);
  
   
}


int main(){
   
    semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //creating shared memory for balance and semaphore variable so that child processes can access and modify them
    balance = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //creating shared memory for balance and semaphore variable so that child processes can access and modify them

   sem_init(semaphore,1,1); //semaphore initilialized to be shared among multiple processes with an initial value of 1.
   
    printf("Enter the balance in your account: \n");
    scanf("%d", balance);
   
	fpt=fopen("account.dat","w");
	fprintf(fpt, "Balance: %d", *balance);
    fclose(fpt);

    fpt=fopen("transaction.dat","w");
    fprintf(fpt, "Process ID: %d, Parent Process ID: %d, Transaction type: DEPOSIT, Prev. balance: 0, Current balance: %d\n", getpid(), getppid(), *balance); 
    fclose(fpt);

    
    pid_t child_deposit = fork();
    if (child_deposit == 0){
        deposit(40);
        exit(0);
    }
    
    else {
        pid_t child_withdrawal = fork();
    
        if (child_withdrawal == 0){
        
            withdraw(20);
            exit(0);
        
         }
    }
  
    sem_destroy(semaphore);

    return 0;
}
