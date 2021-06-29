#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<time.h>
#include "structures.h"
char adminName[64];

// Declaration of the function
void deleteAccount(bool fitstToDel, unsigned long accNo, int cfd);
void deleteCustomer(bool fitstToDel, unsigned long accNo, int cfd);

void setAdminName(char *admin){
    strcpy(adminName, admin);
}

unsigned long getuuid(){
    char str[16];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(str, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return strtoul(str, NULL, 10);   
}

bool isvalidUserName(char *userName){
    
     // Check that username is already present in db or not.
    int fd_cust = open("./db/Customer.txt", O_APPEND | O_RDWR, 0644);
    struct Customer tmpcust;
    int n = 1;

    while(1){
        n = read(fd_cust, &tmpcust, sizeof(tmpcust));
        if(n){
            if(strcmp(userName, tmpcust.userName) == 0){
                // printf("That username is already taken ☹️ \n");
                close(fd_cust);
                return false;
            }
        }
        else{
            close(fd_cust);
            return true;              
        }
    }
}

unsigned long createAccount(int type){

    int fd_acc = open("./db/Account.txt", O_APPEND | O_RDWR, 0644);
    struct Account acc;
    acc.accountNo = getuuid();
    acc.balance = 0;
    acc.type = (type == 2) ? Joint : Normal;

    int status = write(fd_acc, &acc, sizeof(acc));      // Write acc in file

    if(status == -1){
        // perror("Error: ");
        exit(0);
    }
    else{
        char file[64];
        sprintf(file,"./db/Transaction/%llu.txt", acc.accountNo);
        creat(file, 0644);
    }

    close(fd_acc);
    return acc.accountNo;
}

void addCustomer(int cfd){

    int fd_cust = open("./db/Customer.txt", O_APPEND | O_RDWR, 0644);

    int type, t;
    char buff[1024];
    strcpy(buff,"Which Type of Account you want to open?\n  Press 1 for Normal\n  Press 2 for Joint\n");
    write(cfd, buff, sizeof(buff));
    read(cfd, &type, sizeof(type));

    if(type == 2){
        struct Customer cust[2];
        int loop = 2;         // to take info of 2 users for joint acc.
        
        while(loop--){
            char userName[64], password[64];
            sprintf(buff, "Enter Details of User %d", 2-loop);
            write(cfd, buff, sizeof(buff));
            
            while(1){
                strcpy(buff,"Enter UserName and Password: ");
                write(cfd, buff, sizeof(buff));
                read(cfd, userName, sizeof(userName));
                // printf("in\n");
                if(!isvalidUserName(userName) || (loop == 0 && strcmp(cust[1].userName, userName) == 0)){
                    t = -1;
                    write(cfd, &t, sizeof(t));
                    strcpy(buff, "That username is already taken ☹️ \n");
                    write(cfd, buff, sizeof(buff));                    
                }
                else{
                    // printf("valid \n");
                    t = 0;
                    write(cfd, &t, sizeof(t));
                    break;
                }
            }
            strcpy(cust[loop].userName, userName);
            
            strcpy(buff, "Enter Password: ");
            write(cfd, buff, sizeof(buff));
            read(cfd, password, sizeof(password));
            strcpy(cust[loop].password, password);

        } 

        unsigned long accNo = createAccount(type);
        cust[0].accountNo = accNo;
        cust[1].accountNo = accNo;

        int status = write(fd_cust, &cust[0], sizeof(cust[0]));       // write customer in file
        int status1 = write(fd_cust, &cust[1], sizeof(cust[1]));       // write customer in file
        if(status1 == -1|| status == -1){
            t = -1;
            write(cfd, &t, sizeof(t));
            exit(0);
        }else{

            // Admin Logs
            int fd_logs = open("./db/AdminLogs.txt", O_APPEND | O_RDWR, 0644);
            struct AdminLog log;
            log.accountNo = accNo;
            strcpy(log.custUserName, cust[0].userName);
            log.operation = Add;
            log.time = time(NULL);
            strcpy(log.userName, adminName);
            write(fd_logs, &log, sizeof(log)); 

            log.accountNo = accNo;
            strcpy(log.custUserName, cust[1].userName);
            log.operation = Add;
            log.time = time(NULL);
            strcpy(log.userName, adminName);
            write(fd_logs, &log, sizeof(log)); 
            
            close(fd_logs);

            t = 0;
            write(cfd, &t, sizeof(t));
        }

    }
    else{
        struct Customer cust;
        char userName[64], password[64];
        strcpy(buff, "Enter Details of User ");
        write(cfd, buff, sizeof(buff));
        
        while(1){
            strcpy(buff,"Enter UserName and Password: ");
            write(cfd, buff, sizeof(buff));
            read(cfd, userName, sizeof(userName));

            if(!isvalidUserName(userName)){
                t = -1;
                write(cfd, &t, sizeof(t));
                strcpy(buff, "That username is already taken ☹️ \n");
                write(cfd, buff, sizeof(buff));
            }
            else{
                t = 0;
                write(cfd, &t, sizeof(t));
                break;
            }
        }
        strcpy(cust.userName, userName);


        strcpy(buff, "\nEnter Password : ");
        write(cfd, buff, sizeof(buff));  

        // printf("Valid\n");

        read(cfd, password, sizeof(password));
        strcpy(cust.password, password);
        // printf("%s \n", password);

        cust.accountNo = createAccount(type);          // Create acc and bind with customer  
        int status = write(fd_cust, &cust, sizeof(cust));       // write customer in file
        if(status == -1){
            t = -1;
            write(cfd, &t, sizeof(t));
            exit(0);
        }else
        {
            // Admin Logs
            int fd_logs = open("./db/AdminLogs.txt", O_APPEND | O_RDWR, 0644);
            struct AdminLog log;
            log.accountNo = cust.accountNo;
            strcpy(log.custUserName, cust.userName);
            log.operation = Add;
            log.time = time(NULL);
            strcpy(log.userName, adminName);
            write(fd_logs, &log, sizeof(log)); 
            close(fd_logs);

            t = 0;
            write(cfd, &t, sizeof(t));
        }
        
    }
    close(fd_cust);
}


void deleteCustomer(bool fitstToDel, unsigned long accNo, int cfd){        

    int t;
    char buff[1024];

    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    int fd = open("./db/tmpCustomer.txt", O_CREAT | O_RDWR, 0644);
    struct Customer cust;
    bool isThere = false;

    if(fitstToDel){
        char userName[64];
        strcpy(buff, "Enter userName : ");
        write(cfd, buff, sizeof(buff));
        read(cfd, userName, sizeof(userName));

        while(1){
            int n = read(fd_cust, &cust, sizeof(cust));
            if(n){
                if(strcmp(cust.userName, userName) != 0)
                    write(fd, &cust, sizeof(cust));      
                else{
                    isThere = true;
                    accNo = cust.accountNo;
                }
            }
            else
                break;
        }
        if(isThere){
            t = 0;
            write(cfd, &t, sizeof(t));
            deleteAccount(false, accNo, cfd);

            // Admin Logs
            int fd_logs = open("./db/AdminLogs.txt", O_APPEND | O_RDWR, 0644);
            struct AdminLog log;
            log.accountNo = cust.accountNo;
            strcpy(log.custUserName, userName);
            log.operation = Delete;
            log.time = time(NULL);
            strcpy(log.userName, adminName);
            write(fd_logs, &log, sizeof(log)); 
            close(fd_logs);
        }
        else{
            t = -1;
            write(cfd, &t, sizeof(t));
            strcpy(buff, "No such userName is there to delete. \n");
            write(cfd, buff, sizeof(buff));
        }
    }else{
        while(1){
            int n = read(fd_cust, &cust, sizeof(cust));
            if(n){
                if(cust.accountNo != accNo){
                    write(fd, &cust, sizeof(cust));
                }
                else{
                    // Admin Logs
                    int fd_logs = open("./db/AdminLogs.txt", O_APPEND | O_RDWR, 0644);
                    struct AdminLog log;
                    log.accountNo = cust.accountNo;
                    strcpy(log.custUserName, cust.userName);
                    log.operation = Delete;
                    log.time = time(NULL);
                    strcpy(log.userName, adminName);
                    write(fd_logs, &log, sizeof(log)); 
                    close(fd_logs);
                }
            }      
            if(n == 0)
                break;
        }
    }
    
    close(fd_cust);
    close(fd);
    system("rm ./db/Customer.txt");
    system("mv ./db/tmpCustomer.txt ./db/Customer.txt");
}

void deleteAccount(bool fitstToDel, unsigned long accNo, int cfd){  

    int deltyp;
    int t;
    char buff[1024];

    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    int fd = open("./db/tmpAccount.txt", O_CREAT | O_RDWR, 0644);
    struct Account acc;
    bool isThere = false;
    bool delTxn = true;
    
    if(fitstToDel){
        strcpy(buff, "Enter Account No : ");
        write(cfd, buff, sizeof(buff));
        read(cfd, &accNo, sizeof(accNo));
    }
    while(1){
        int n = read(fd_acc, &acc, sizeof(acc));
        if(n){
            if(acc.accountNo != accNo)
                write(fd, &acc, sizeof(acc));
            else if(!fitstToDel && acc.accountNo == accNo && acc.type == Joint){    // When we delete Joint acc. one holder
                acc.type = Normal;
                write(fd, &acc, sizeof(acc));
                isThere = true;
                delTxn = false;
            }
            else
                isThere = true;
        }else
            break;
    }

    if(delTxn){
        char file[64];
        sprintf(file,"rm ./db/Transaction/%lu.txt", accNo);
        system(file);
    }

    if(isThere && fitstToDel){
        t = 0;
        write(cfd, &t, sizeof(t));
        deleteCustomer(false ,accNo, cfd); 
    }
    else if(!isThere){
        t = -1;
        write(cfd, &t, sizeof(t));
        strcpy(buff, "No such account number is there to delete. \n");
        write(cfd, buff, sizeof(buff));
    }
    
    close(fd_acc);
    close(fd);
    system("rm ./db/Account.txt");
    system("mv ./db/tmpAccount.txt ./db/Account.txt");
}

void delete(int cfd){
    int t;
    char buff[1024];

    strcpy(buff, "CAUSION : CHECK ACCOUNT BALANCE BEFORE YOU PROCEED FORWARD. \nPress 0 to exit.\n");
    strcat(buff, "Press 1 to Delete By Account NO.\n");
    strcat(buff, "Press 2 to Delete By UserName. \n");
    write(cfd, buff, sizeof(buff));
    read(cfd, &t, sizeof(t));

    if(t == 1){
        deleteAccount(true, 0, cfd);
    }else if(t == 2){
        deleteCustomer(true, 0, cfd);
    }
}


void updateCustomer(int cfd){

    int t;
    char buff[1024];
    char tmpuserName[64];
    char userName[64], password[64];

    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    struct Customer cust;
    
    
    strcpy(buff, "Enter userName : ");
    write(cfd, buff, sizeof(buff));
    read(cfd, tmpuserName, sizeof(tmpuserName));

    while(1){
        int n = read(fd_cust, &cust, sizeof(cust));
        if(n){
            if(strcmp(cust.userName, tmpuserName) == 0){
                t = 0;
                write(cfd, &t, sizeof(t));
                
                struct flock lck;

                lck.l_type = F_WRLCK;
                lck.l_whence = SEEK_CUR;
                lck.l_start = -sizeof(fd_cust);
                lck.l_len = sizeof(fd_cust);
                lck.l_pid = getpid();

                // lock    
                int s=0;
                s = fcntl(fd_cust, F_SETLK, &lck);
                write(cfd, &s, sizeof(s));
                if(s == -1){
                strcpy(buff, "Your Account is in use...");
                write(cfd, buff, sizeof(buff));
                }

                while(s == -1){
                s = fcntl(fd_cust, F_SETLK, &lck);    
                }

                lseek(fd_cust, -sizeof(cust), SEEK_CUR);
                read(fd_cust, &cust, sizeof(cust));

                sleep(1);

                while(1){
                    int tt;
                    strcpy(buff, "Enter New UserName : ");
                    write(cfd, buff, sizeof(buff));
                    read(cfd, userName, sizeof(userName));
                    
                    if(isvalidUserName(userName)){
                        tt = 0;
                        write(cfd, &tt, sizeof(tt));
                        break;
                    }else{
                        tt = -1;
                        write(cfd, &tt, sizeof(tt));
                        strcpy(buff, "That username is already taken ☹️ \n");
                        write(cfd, buff, sizeof(buff));                    
                    }
                }
                strcpy(cust.userName, userName);

                strcpy(buff, "Enter New Password : ");
                write(cfd, buff, sizeof(buff));
                read(cfd, password, sizeof(password));
                strcpy(cust.password, password);

                lseek(fd_cust, -sizeof(cust), SEEK_CUR);
                write(fd_cust, &cust, sizeof(cust));

                // Unlock
                fcntl(fd_cust, F_UNLCK, &lck);


                close(fd_cust);
        
                // Admin Logs
                int fd_logs = open("./db/AdminLogs.txt", O_APPEND | O_RDWR, 0644);
                struct AdminLog log;
                log.accountNo = cust.accountNo;
                strcpy(buff, tmpuserName);
                strcat(buff, "->");
                strcat(buff, cust.userName);
                strcpy(log.custUserName, buff);
                log.operation = Update;
                log.time = time(NULL);
                strcpy(log.userName, adminName);
                write(fd_logs, &log, sizeof(log)); 
                close(fd_logs);


                break;
            }
        }
        else{
            t = -1;
            write(cfd, &t, sizeof(t));
            strcpy(buff, "No such userName is there to update. \n");
            write(cfd, buff, sizeof(buff));
            break;
        }
    }
   
    close(fd_cust);
}


int searchCustomerByUserName(char *userName, struct Customer* ptr[]){
    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    struct Customer cust;
    int ind=0;
    while(1){
        int n = read(fd_cust, &cust, sizeof(cust));
        if(n && (strcmp(cust.userName, userName) == 0)){
            ptr[ind]->accountNo = cust.accountNo;
            strcpy(ptr[ind]->password, cust.password);
            strcpy(ptr[ind]->userName, cust.userName);
            ind++;
        }
        else if(n == 0){
            if(ind == 0){
                // printf("No such userName is there.\n");
                ptr = NULL;
                return -1;
            }else
                return 0;
        }
    }     
}   

int searchCustomerByAccountNo(unsigned long accNo, struct Customer* ptr[]){
    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    struct Customer cust;
    int ind = 0;
    while(1){
        int n = read(fd_cust, &cust, sizeof(cust));
        if(n && (cust.accountNo == accNo)){
            ptr[ind]->accountNo = cust.accountNo;
            strcpy(ptr[ind]->password, cust.password);
            strcpy(ptr[ind]->userName, cust.userName);
            ind++;
        }
        else if(n == 0){
            if(ind == 0){
                // printf("No such userName is there.\n");
                ptr = NULL;
                return -1;
            }else
                return 0;
        }
    }     
}  

int searchAccount(unsigned long accNo, struct Account *ptr){
    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    struct Account acc;

   while(1){
        int n = read(fd_acc, &acc, sizeof(acc));
        if(n && (acc.accountNo == accNo)){
            ptr->accountNo = acc.accountNo;
            ptr->balance = acc.balance;
            ptr->type = acc.type;
            return 0;
        }
        else if(n == 0){
            // printf("No such accountNumber is there. \n");
            ptr = NULL;
            return -1;
        }
    }
}  

void search(int cfd){
    struct Customer *cust[2];
    cust[0] = (struct Customer*)malloc(sizeof(struct Customer));
    cust[1] = (struct Customer*)malloc(sizeof(struct Customer));
    struct Account *acc = (struct Account*)malloc(sizeof(struct Account));;
    
    int t;
    bool isThere = true;
    char buff[1024];
    char userName[64], password[64];

    strcpy(buff, "Press 1 to Search By UserName. \nPress 2 to Search By Account NO. \n");
    write(cfd, buff, sizeof(buff));
    read(cfd, &t, sizeof(t));

    if(t == 1){
        char userName[64];
        strcpy(buff, "Enter userName : ");
        write(cfd, buff, sizeof(buff));
        read(cfd, userName, sizeof(userName));
        
        int sc = searchCustomerByUserName(userName, cust);
        int sa = searchAccount(cust[0]->accountNo, acc);
        if(sa==-1 || sc==-1){
            isThere = false;
            
            t = -1;
            write(cfd, &t, sizeof(t));
            strcpy(buff, "No such userName is there. 😕 \n");
            write(cfd, buff, sizeof(buff));
            
        }else{
            t = 0;
            write(cfd, &t, sizeof(t));
        }

    }
    else{
        unsigned long accNo;
        strcpy(buff, "Enter Account No : ");
        write(cfd, buff, sizeof(buff));
        read(cfd, &accNo, sizeof(accNo));
        
        int sa = searchAccount(accNo, acc);
        int sc = searchCustomerByAccountNo(accNo, cust);
        if(sa==-1 || sc==-1){
            isThere = false;
            
            t = -1;
            write(cfd, &t, sizeof(t));
            strcpy(buff, "No such Account No is there. 😕 \n");
            write(cfd, buff, sizeof(buff));
        }else{
            t = 0;
            write(cfd, &t, sizeof(t));
        }
            
    }

    
    if(isThere){
        int ind = (acc->type == 0) ? 2 : 1;
        char tmp[128];
        strcpy(buff,"----------------------------- Details -----------------------------\n\n");
        while(ind--){
            sprintf(tmp, "Customer UserName \t: %s\n", cust[ind]->userName);
            strcat(buff, tmp);
            sprintf(tmp, "Customer Password \t: %s\n", cust[ind]->password);
            strcat(buff, tmp);
            sprintf(tmp, "Customer Account No \t: %llu\n", cust[ind]->accountNo);
            strcat(buff, tmp);
            sprintf(tmp, "Customer Balance \t: %f\n", acc->balance);
            strcat(buff, tmp);
            sprintf(tmp, "Customer Type \t\t: %s\n\n", (acc->type == 0) ? "Joint" : "Normal"); 
            strcat(buff, tmp);
        }
        strcat(buff,"-----------------------------------------------------------------\n");
        write(cfd, buff, sizeof(buff));
    }

}


void viewDetails(unsigned long accNo, int cfd){

    struct Transaction txn;
    char buff[1024*10];
    char str[1024];
    int t;
    
    char file[64];
    sprintf(file,"./db/Transaction/%lu.txt", accNo);
    int fd_txn = open(file, O_RDWR, 0644);
    
    if(fd_txn != -1){
        strcpy(buff, "UserName AccountNo Amount Type Balance Date_Time\n");
        
        while(read(fd_txn, &txn, sizeof(txn))){
            sprintf(str,"%s %lld %.2f %s %.2lf %s \n", txn.userName, txn.accountNo, txn.amount, (txn.type == 0) ? "+Deposit" : "-Withdraw", txn.finalBalance, ctime(&txn.time));
            strcat(buff, str);
        }
        t = 0;
        write(cfd, &t, sizeof(t));
        write(cfd, buff, sizeof(buff));
    }else{
        t = -1;
        write(cfd, &t, sizeof(t));
        strcpy(buff, "No Such Account No is there in DB. \n\n");
        write(cfd, buff, sizeof(buff));
    }
    close(fd_txn);
}

void balanceEnquiry(unsigned long accNo, int cfd){

    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    struct Account acc;
    char buff[1024];

     while(1){
        int n = read(fd_acc, &acc, sizeof(acc));
        if(n && (acc.accountNo == accNo)){  

            struct flock lck;

            lck.l_type = F_RDLCK;
            lck.l_whence = SEEK_CUR;
            lck.l_start = -sizeof(acc);
            lck.l_len = sizeof(acc);
            lck.l_pid = getpid();

            // lock    
            int s=0;
            s = fcntl(fd_acc, F_SETLK, &lck);
            write(cfd, &s, sizeof(s));
            if(s == -1){
                strcpy(buff, "Your Account is in use...");
                write(cfd, buff, sizeof(buff));
            }
            
            sleep(4);

            while(s == -1){
                s = fcntl(fd_acc, F_SETLK, &lck);    
            }

            sleep(1);


            sprintf(buff, "Current Balance : %f\n\n", acc.balance);
            write(cfd, buff, sizeof(buff));

            close(fd_acc);

            break;
        }
    }

}


void pwd_name_Change(unsigned long accNo, int cfd){

    int t;
    char buff[1024];
    char tmpuserName[64];
    char userName[64], password[64];

    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    struct Customer cust;
    
    strcpy(buff, "Enter userName : ");
    write(cfd, buff, sizeof(buff));
    read(cfd, tmpuserName, sizeof(tmpuserName));

    while(1){
        int n = read(fd_cust, &cust, sizeof(cust));
        if(n){
            if(strcmp(cust.userName, tmpuserName) == 0 && (cust.accountNo == accNo)){
                t = 0;
                write(cfd, &t, sizeof(t));

                struct flock lck;

                lck.l_type = F_WRLCK;
                lck.l_whence = SEEK_CUR;
                lck.l_start = -sizeof(fd_cust);
                lck.l_len = sizeof(fd_cust);
                lck.l_pid = getpid();

                // lock    
                int s=0;
                s = fcntl(fd_cust, F_SETLK, &lck);
                write(cfd, &s, sizeof(s));
                if(s == -1){
                strcpy(buff, "Your Account is in use...");
                write(cfd, buff, sizeof(buff));
                }

                while(s == -1){
                s = fcntl(fd_cust, F_SETLK, &lck);    
                }

                lseek(fd_cust, -sizeof(cust), SEEK_CUR);
                read(fd_cust, &cust, sizeof(cust));

                sleep(1);

                
                while(1){
                    int tt;
                    strcpy(buff, "Enter New UserName : ");
                    write(cfd, buff, sizeof(buff));
                    read(cfd, userName, sizeof(userName));
                    
                    if(isvalidUserName(userName)){
                        tt = 0;
                        write(cfd, &tt, sizeof(tt));
                        break;
                    }else{
                        tt = -1;
                        write(cfd, &tt, sizeof(tt));
                        strcpy(buff, "That username is already taken ☹️ \n");
                        write(cfd, buff, sizeof(buff));                    
                    }
                }
                strcpy(cust.userName, userName);

                strcpy(buff, "Enter New Password : ");
                write(cfd, buff, sizeof(buff));
                read(cfd, password, sizeof(password));
                strcpy(cust.password, password);

                lseek(fd_cust, -sizeof(cust), SEEK_CUR);
                write(fd_cust, &cust, sizeof(cust));

               // Unlock
                fcntl(fd_cust, F_UNLCK, &lck);

                close(fd_cust);

                break;
            }
        }
        else{
            t = -1;
            write(cfd, &t, sizeof(t));
            strcpy(buff, "You have entered Wrong userName \n  Please Try again later.\n");
            write(cfd, buff, sizeof(buff));
            break;
        }
    }
   
    close(fd_cust);

}



void transactionEntry(unsigned long accNo, float amt, unsigned int type, double finalBalance, char *userName){

    
    struct Transaction txn;
    char file[64];
    sprintf(file,"./db/Transaction/%lu.txt", accNo);
    int fd_txn = open(file, O_CREAT | O_APPEND | O_RDWR, 0644);

    txn.accountNo = accNo;
    strcpy(txn.userName, userName);
    txn.time = time(NULL);
    txn.amount = amt;
    txn.type = type;
    txn.finalBalance = finalBalance;

    write(fd_txn, &txn, sizeof(txn));

    close(fd_txn);
}


void withdraw(unsigned long accNo, int cfd, char *userName){

    int t;
    char buff[1024];
    double amt;

    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    struct Account acc;

    strcpy(buff, "How much do you want to withdraw? ");
    write(cfd, buff, sizeof(buff));
    read(cfd, &amt, sizeof(amt));

    while(1){
        int n = read(fd_acc, &acc, sizeof(acc));
        if(n && (acc.accountNo == accNo)){
            
            if(acc.balance - amt < 0){
                sprintf(buff, "      Insufficient Account Balance : %f\n   You can withdraw upto mentioned balance.\n", acc.balance);
                write(cfd, buff, sizeof(buff));
            }
            else{

                struct flock lck;

                lck.l_type = F_WRLCK;
                lck.l_whence = SEEK_CUR;
                lck.l_start = -sizeof(acc);
                lck.l_len = sizeof(acc);
                lck.l_pid = getpid();


                // lock    
                int s=0;
                s = fcntl(fd_acc, F_SETLK, &lck);
                write(cfd, &s, sizeof(s));
                if(s == -1){
                    strcpy(buff, "Your Account is in use...");
                    write(cfd, buff, sizeof(buff));
                }

                sleep(4);

                while(s == -1){
                    s = fcntl(fd_acc, F_SETLK, &lck);    
                }

                lseek(fd_acc, -sizeof(acc), SEEK_CUR);
                read(fd_acc, &acc, sizeof(acc));

                sleep(1);

                acc.balance = acc.balance - amt;
                sprintf(buff, "Updated Account Balance : %f", acc.balance);
                write(cfd, buff, sizeof(buff));

                lseek(fd_acc, -sizeof(acc), SEEK_CUR);
                write(fd_acc, &acc, sizeof(acc));

                // Unlock
                fcntl(fd_acc, F_UNLCK, &lck);


                transactionEntry(accNo, amt, 1, acc.balance, userName);

                close(fd_acc);

            }
            break;
        }
    }
}


void deposit(unsigned long accNo, int cfd, char *userName){

    int t;
    char buff[1024];
    double amt;

    int fd_acc = open("./db/Account.txt", O_RDWR, 0644);
    struct Account acc;

    strcpy(buff, "How much do you want to deposit? ");
    write(cfd, buff, sizeof(buff));
    read(cfd, &amt, sizeof(amt));


    while(1){
        int n = read(fd_acc, &acc, sizeof(acc));

        if(n && (acc.accountNo == accNo)){

            struct flock lck;

            lck.l_type = F_WRLCK;
            lck.l_whence = SEEK_CUR;
            lck.l_start = -sizeof(acc);
            lck.l_len = sizeof(acc);
            lck.l_pid = getpid();


            // lock    
            int s=0;
            s = fcntl(fd_acc, F_SETLK, &lck);
            write(cfd, &s, sizeof(s));
            if(s == -1){
                strcpy(buff, "Your Account is in use...");
                write(cfd, buff, sizeof(buff));
            }
            
            sleep(4);

            while(s == -1){
                s = fcntl(fd_acc, F_SETLK, &lck);    
            }

            lseek(fd_acc, -sizeof(acc), SEEK_CUR);
            read(fd_acc, &acc, sizeof(acc));

            sleep(1);

            acc.balance = acc.balance + amt;
            sprintf(buff, "Updated Account Balance : %f", acc.balance);
            write(cfd, buff, sizeof(buff));

            lseek(fd_acc, -sizeof(acc), SEEK_CUR);
            write(fd_acc, &acc, sizeof(acc));

            // Unlock
            fcntl(fd_acc, F_UNLCK, &lck);

            transactionEntry(accNo, amt, 0, acc.balance, userName);

            close(fd_acc);
            

            break;
        }
    }
}


