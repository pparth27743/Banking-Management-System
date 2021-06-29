#include<stdlib.h>
#include<stdbool.h>
#include<time.h>

struct Customer{
    char userName[64];
    char password[16];
    unsigned long long accountNo;
};

struct Admin{
    char userName[64];
    char password[16];
};


struct Account{
    unsigned long long accountNo;
    double balance;
    enum {Joint, Normal} type;
};


struct Transaction{
    char userName[64];
    unsigned long long accountNo;
    time_t time;
    float amount;
    double finalBalance;
    enum {Deposit, Withdraw} type;                      
};


struct AdminLog{
    char userName[64];
    time_t time;
    enum {Add, Delete, Update} operation;
    char custUserName[64];
    unsigned long long accountNo;
};

