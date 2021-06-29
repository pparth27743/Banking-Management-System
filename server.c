#include<fcntl.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/sem.h>
#include<arpa/inet.h>

#include "operation.c"

int cfd;

void adminOperations(){
    
    while(1){
        int t;
        char buff[1024] = "----------------------------- Menu -----------------------------\n";
        strcat(buff, "Press 1 to add an Accout \nPress 2 to delete Account/Customer\n");
        strcat(buff, "Press 3 to Modify Customer details \nPress 4 to Search Customer/Account\n");
        strcat(buff, "Press 0 to exit\n");
        write(cfd, buff, sizeof(buff));
        read(cfd, &t, sizeof(t));

        switch (t)
        {
            case 1:
                addCustomer(cfd);
                break;
            
            case 2:
                delete(cfd);
                break;
            
            case 3:
                updateCustomer(cfd);
                break;
            
            case 4:
                search(cfd);
                break;
            
            default:
                return;
                break;
        }
    }
}

int adminLogin(){

    int fd_admin = open("./db/Admin.txt", O_RDWR, 0644);
    struct Admin adm;
    char userName[64], password[64];
    char buff[1024] = "Enter following credentials for Admin Login. \nEnter UserName : ";
    write(cfd, buff, sizeof(buff));
    read(cfd, userName, sizeof(userName));
    
    strcpy(buff, "Enter Password : ");
    write(cfd, buff, sizeof(buff));
    read(cfd, password, sizeof(password));

    while(1){
        int n = read(fd_admin, &adm, sizeof(adm));
        if(n){
            if((strcmp(adm.userName, userName) == 0) && (strcmp(adm.password, password) == 0)){
                strcpy(buff, "-------------------------- Welcome ");
                strcat(buff, adm.userName);
                strcat(buff, " -------------------------- \n");
                write(cfd, buff, sizeof(buff));
                int t = 0;
                write(cfd, &t, sizeof(t));              // Send successful login signal
                setAdminName(adm.userName);
                return 0;
            }
             
        }
        else{
            strcpy(buff, "Incorrect UserName or Password. \nPleaes try again\n");
            write(cfd, buff, sizeof(buff));
            int t = -1;
            write(cfd, &t, sizeof(t));                  // Send unsuccessful login signal
            return -1;
        }
    }
}


void customerOperations(unsigned long accNo, char *userName){
     while(1){
        int t;
        char buff[1024] = "----------------------------- Menu -----------------------------\n";
        strcat(buff, "Press 1 to Deposit \nPress 2 to Withdraw\n");
        strcat(buff, "Press 3 for Balance Enquiry  \nPress 4 to Password/UserName Change\n");
        strcat(buff, "Press 5 to View details\nPress 0 to exit\n");
        write(cfd, buff, sizeof(buff));
        read(cfd, &t, sizeof(t));

        switch (t)
        {
            case 1:
                deposit(accNo, cfd, userName);
                break;
            
            case 2:
                withdraw(accNo, cfd, userName);
                break;
            
            case 3:
                balanceEnquiry(accNo, cfd);
                break;
            
            case 4:
                pwd_name_Change(accNo, cfd);
                break;
            
            case 5:
                viewDetails(accNo, cfd);
                break;

            default:
                return;
                break;
        }
    }
}


unsigned long long customerLogin(char *usr){

    int fd_cust = open("./db/Customer.txt", O_RDWR, 0644);
    struct Customer cust;
    char userName[64], password[64];
    char buff[1024] = "Enter following credentials for Customer Login. \nEnter UserName : ";
    write(cfd, buff, sizeof(buff));
    read(cfd, userName, sizeof(userName));
    
    strcpy(buff, "Enter Password : ");
    write(cfd, buff, sizeof(buff));
    read(cfd, password, sizeof(password));

    while(1){
        int n = read(fd_cust, &cust, sizeof(cust));
        if(n){
            if((strcmp(cust.userName, userName) == 0) && (strcmp(cust.password, password) == 0)){
                strcpy(buff, "-------------------------- Welcome ");
                strcat(buff, cust.userName);
                strcat(buff, " -------------------------- \n");
                write(cfd, buff, sizeof(buff));
                int t = 0;
                write(cfd, &t, sizeof(t));              // Send successful login signal
                strcpy(usr, cust.userName);
                return cust.accountNo;
            }
        }
        else{
            strcpy(buff, "Incorrect UserName or Password. \nPleaes try again\n");
            write(cfd, buff, sizeof(buff));
            int t = -1;
            write(cfd, &t, sizeof(t));                  // Send unsuccessful login signal
            return 0;
        }
    }

}

void homepage(){

    char buff[1024] = "\n============================ Welcome to ONLINE BANKING SYSTEM ============================\n";
    strcat(buff, "Press 1 for Admin Login \nPress 2 for Customer Login \n");
    write(cfd, buff, sizeof(buff));
    int res;
    read(cfd, &res, sizeof(res));
    int adminLoginStat = -1;
    unsigned long long accNo;
    char userName[64];

    if(res == 1){
        adminLoginStat = adminLogin();
        if(adminLoginStat == 0)
            adminOperations();
    }
    else{
        accNo = customerLogin(userName);
        if(accNo != 0)
            customerOperations(accNo, userName);
    }
}

int main(){
    struct sockaddr_in server, clinet;
    int sfd;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);

    bind(sfd, (struct sockaddr*)&server, sizeof(server)); 
    listen(sfd, 5);

    
    while(1){
        int len = sizeof(clinet);
        cfd = accept(sfd, (struct sockaddr*)&clinet, &len);

        if(!fork()){
            close(sfd);

            homepage();
            
            close(cfd);
            exit(0);
        }else {
            sleep(3);
            close(cfd);
        }
    }
    close(sfd);
    return 0;
}
