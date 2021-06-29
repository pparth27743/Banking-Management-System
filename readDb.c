#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include "structures.h"

void printAdmins(){

    struct Admin adm;
    int n = 1;
    char str[1024];

    int fd_adm = open("./db/Admin.txt", O_RDWR, 0644);
    int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);

    write(fd_tmp, "UserName Password \n", strlen("UserName Password \n"));
    while(1){
        n = read(fd_adm, &adm, sizeof(adm));
        if(n){
            sprintf(str,"%s %s\n",adm.userName, adm.password);
            write(fd_tmp, str, strlen(str));
        }
        else
            break;  
    }
    
    printf("\n");
    system("cat .tmp | column -t");       
    system("rm .tmp");   
    printf("\n\n");
    close(fd_tmp);
    close(fd_adm);

}

void printAccounts(){

    struct Account acc;
    int n = 1;
    char str[1024];

    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);

    write(fd_tmp, "AccountNo Balance Type\n", strlen("AccountNo Balance Type\n"));
    while(1){
        n = read(fd_acc, &acc, sizeof(acc));
        if(n){
            sprintf(str,"%lld %.2f %s\n",acc.accountNo, acc.balance, (acc.type == 0) ? "Joint" : "Normal");
            write(fd_tmp, str, strlen(str));
        }
        else
            break;  
    }
    
    printf("\n");
    system("cat .tmp | column -t");       
    system("rm .tmp");   
    printf("\n\n");
    close(fd_tmp);
    close(fd_acc);
}

void printCustomers(){
    struct Customer cust;
    int n = 1;
    char str[1024];
    
    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);

    write(fd_tmp, "UserName Password AccountNo \n", strlen("UserName Password AccountNo \n"));
    while(1){
        n = read(fd_cust, &cust, sizeof(cust));
        if(n){
            sprintf(str,"%s %s %lld\n",cust.userName, cust.password, cust.accountNo);
            write(fd_tmp, str, strlen(str));
        }
        else
            break;  
    }
    
    printf("\n");
    system("cat .tmp | column -t");       
    system("rm .tmp");   
    printf("\n\n");
    close(fd_tmp);
    close(fd_cust);
}

void printTrasactions(){

    struct Transaction txn;
    unsigned long accountNo;
    int n = 1;
    char str[1024];
    
    printf("Enter Account No. : ");
    scanf("%lu", &accountNo);

    char file[64];
    sprintf(file,"./db/Transaction/%lu.txt", accountNo);
    int fd_txn = open(file, O_RDWR, 0644);
    
    if(fd_txn != -1){

        int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);
        write(fd_tmp, "UserName AccountNo Amount Type Balance Date_Time\n", strlen("UserName AccountNo Amount Type Balance Date_Time\n"));
        
        while(read(fd_txn, &txn, sizeof(txn))){
            sprintf(str,"%s %lld %.2f %s %.2lf %s \n", txn.userName, txn.accountNo, txn.amount, (txn.type == 0) ? "+Deposit" : "-Withdraw", txn.finalBalance, ctime(&txn.time));
            write(fd_tmp, str, strlen(str));
        }

        printf("\n");
        system("cat .tmp | column -t");       
        system("rm .tmp");   
        printf("\n\n");
        close(fd_tmp);
        close(fd_txn);


    }else{
        printf("No Such Account No is there in DB. \n\n");
    }
}

void printAdminLogs(){

    struct AdminLog log;
    int n = 1;
    char str[1024];
    char op[10];

    int fd_logs = open("./db/AdminLogs.txt", O_RDWR, 0644);
    int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);

    write(fd_tmp, "AdminName Operation Cust_Name Cust_accNo Date_Time\n", strlen("AdminName Operation Cust_Name Cust_accNo Date_Time\n"));
    while(1){
        n = read(fd_logs, &log, sizeof(log));
        if(n){
            strcpy(op, (log.operation == Add) ? "Add": (log.operation == Delete ? "Delete": "Update"));
            sprintf(str,"%s %s %s %llu %s\n", log.userName, op, log.custUserName, log.accountNo, ctime(&log.time));
            write(fd_tmp, str, strlen(str));
        }
        else
            break;  
    }
    
    printf("\n");
    system("cat .tmp | column -t");       
    system("rm .tmp");   
    printf("\n\n");
    close(fd_tmp);
    close(fd_logs);
}


int main(){

    while(1){
        int choice;
        printf("Choose Any Following Option: \n");
        printf("Press 1 Print Accounts \n");
        printf("Press 2 Print Customers \n");
        printf("Press 3 Print Transactions \n");
        printf("Press 4 Print Admins \n");
        printf("Press 5 Print Admin Logs \n");
        printf("Press 0 to Exit \n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printAccounts();
            break;
        
        case 2:
            printCustomers();
            break;
        
        case 3:
            printTrasactions();
            break;
        
        case 4:
            printAdmins();
            break;
        
        case 5:
            printAdminLogs();
            break;
        
        default:
            exit(0);
            break;
        }

    }
    return 0;
}