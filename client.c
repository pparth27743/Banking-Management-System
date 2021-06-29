#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/sem.h>
#include<arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#define MAXPW 64

ssize_t getpasswd (char **pw, size_t sz, int mask, FILE *fp)
{
    if (!pw || !sz || !fp) return -1;       /* validate input   */
#ifdef MAXPW
    if (sz > MAXPW) sz = MAXPW;
#endif

    if (*pw == NULL) {              /* reallocate if no address */
        void *tmp = realloc (*pw, sz * sizeof **pw);
        if (!tmp)
            return -1;
        memset (tmp, 0, sz);    /* initialize memory to 0   */
        *pw =  (char*) tmp;
    }

    size_t idx = 0;         /* index, number of chars in read   */
    int c = 0;

    struct termios old_kbd_mode;    /* orig keyboard settings   */
    struct termios new_kbd_mode;

    if (tcgetattr (0, &old_kbd_mode)) { /* save orig settings   */
        fprintf (stderr, "%s() error: tcgetattr failed.\n", __func__);
        return -1;
    }   /* copy old to new */
    memcpy (&new_kbd_mode, &old_kbd_mode, sizeof(struct termios));

    new_kbd_mode.c_lflag &= ~(ICANON | ECHO);  /* new kbd flags */
    new_kbd_mode.c_cc[VTIME] = 0;
    new_kbd_mode.c_cc[VMIN] = 1;
    if (tcsetattr (0, TCSANOW, &new_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    /* read chars from fp, mask if valid char specified */
    while (((c = fgetc (fp)) != '\n' && c != EOF && idx < sz - 1) ||
            (idx == sz - 1 && c == 127))
    {
        if (c != 127) {
            if (31 < mask && mask < 127)    /* valid ascii char */
                fputc (mask, stdout);
            (*pw)[idx++] = c;
        }
        else if (idx > 0) {         /* handle backspace (del)   */
            if (31 < mask && mask < 127) {
                fputc (0x8, stdout);
                fputc (' ', stdout);
                fputc (0x8, stdout);
            }
            (*pw)[--idx] = 0;
        }
    }
    (*pw)[idx] = 0; /* null-terminate   */

    /* reset original keyboard  */
    if (tcsetattr (0, TCSANOW, &old_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    if (idx == sz - 1 && c != '\n') /* warn if pw truncated */
        fprintf (stderr, " (%s() warning: truncated at %zu chars.)\n",
                __func__, sz - 1);

    return idx; /* number of chars in passwd    */
}


int sfd;

void addCustomer(){
    int t, type;
    char buff[1024];
    char userName[64], password[64];

    // Which type acc
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    scanf("%d", &type);                // enter type Joint/Normal
    write(sfd, &type, sizeof(type));

    while(type--){
        read(sfd, buff, sizeof(buff));      // read "Enter Detais" 
        printf("%s\n", buff);

        while(1){
            read(sfd, buff, sizeof(buff));      // read "Enter UserName"
            printf("%s\n", buff);
            scanf("%s", userName);
            write(sfd, userName, sizeof(userName));       // sent userName
            
            read(sfd, &t, sizeof(t));                     // read whether userName is valid or not
            if(t == 0)
                break;
            else{
                read(sfd, buff, sizeof(buff));      
                printf("%s\n", buff);
            }
        }
        
        read(sfd, buff, sizeof(buff));      // read "Enter Password"
        scanf("%s", password);
        write(sfd, password, sizeof(password));  // sent password
    }
    read(sfd, &t, sizeof(t));                   // Read whether customer created successfully or not.
    
    if(t == -1)
        printf("%s\n", "Error : Customer is not added. \n");
    else
        printf("%s\n", "Customer is added successfully. 💳\n");
}

void delete(){

    int t;
    char buff[1024];
    char userName[64];
    unsigned long accNo;

    read(sfd, buff, sizeof(buff));       // read "delete by AccNo or delete by userName"
    printf("%s\n", buff);              
    scanf("%d", &t);
    write(sfd, &t, sizeof(t));

    if(t == 1 || t == 2){
        if(t == 1){
            read(sfd, buff, sizeof(buff));      // read "Enter Acc No"
            printf("%s\n", buff);               
            scanf("%lu", &accNo);
            write(sfd, &accNo, sizeof(accNo));
        }
        else if(t == 2){
            read(sfd, buff, sizeof(buff));      // read "Enter username"
            printf("%s\n", buff);               
            scanf("%s", userName);
            write(sfd, userName, sizeof(userName));
        }
        read(sfd, &t, sizeof(t));
        if(t == -1){
            read(sfd, buff, sizeof(buff));      // read  erroe msg "no such userName/Account"
            printf("%s\n", buff);               
        }
        else{
            printf("Successfully Deleted.\n\n");               
        }
    }
}

void updateCustomer(){

    int t;
    char buff[1024];
    char userName[64], password[64];

    read(sfd, buff, sizeof(buff));      // read "Enter username"
    printf("%s\n", buff);               
    scanf("%s", userName);
    write(sfd, userName, sizeof(userName));

    read(sfd, &t, sizeof(t));
    if(t == 0){


        // Check critical section available or not
        read(sfd, &t, sizeof(t));

        if(t == -1){     
            read(sfd, buff, sizeof(buff));
            printf("%s\n", buff);  
        }


        int tt;
        while(1){
            read(sfd, buff, sizeof(buff));      // read "Enter new username"
            printf("%s\n", buff);               
            scanf("%s", userName);
            write(sfd, userName, sizeof(userName));
            
            read(sfd, &tt, sizeof(tt));
            if(tt == 0)
                break;
            else{
                read(sfd, buff, sizeof(buff));      // read error msg "userName is already taken"
                printf("%s\n", buff);               
            }
        }

        read(sfd, buff, sizeof(buff));      // read "Enter password"
        printf("%s\n", buff);               
        scanf("%s", password);
        write(sfd, password, sizeof(password));

        printf("Details Updated Successfully.\n\n");
    }
    else{
        read(sfd, buff, sizeof(buff));          // read erroe msg "no such userName"
        printf("%s\n", buff);               
    }
}

void search(){
    
    int t;
    char buff[1024];
    char userName[64];
    unsigned long accNo;

    read(sfd, buff, sizeof(buff));       // read "Serach by Acc/UserName"
    printf("%s\n", buff);              
    scanf("%d", &t);
    write(sfd, &t, sizeof(t));

    if(t == 1){
        read(sfd, buff, sizeof(buff));      // read "Enter username"
        printf("%s\n", buff);               
        scanf("%s", userName);
        write(sfd, userName, sizeof(userName));

        read(sfd, &t, sizeof(t));
        if(t == -1){
            read(sfd, buff, sizeof(buff));      // read  erroe msg "no such userName"
            printf("%s\n", buff);               
        }
    }
    else{
        read(sfd, buff, sizeof(buff));      // read "Enter Acc No"
        printf("%s\n", buff);               
        scanf("%lu", &accNo);
        write(sfd, &accNo, sizeof(accNo));

        read(sfd, &t, sizeof(t));
        if(t == -1){
            read(sfd, buff, sizeof(buff));      // read  erroe msg "No such account no"
            printf("%s\n", buff);               
        }
    }

    if(t == 0){                             // If userName/AccNo is valid then, 
        read(sfd, buff, sizeof(buff));      // read Details of Customer and Account
        printf("%s\n", buff);     
    }
}

void adminClient(){
    char buff[1024];
    int in;
    char password[MAXPW] = {0};
    char *p = password;
    FILE *fp = stdin;
    ssize_t nchr = 0;


    // Enter UserName & Password for admin login and send it for authentication
    char userName[64]; 
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    scanf("%s", userName);
    write(sfd, userName, sizeof(userName));
    
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    getchar();
    nchr = getpasswd (&p, MAXPW, '*', fp);
    write(sfd, password, sizeof(password));
    printf("\n");


    
    // Get Response of Authentication
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    read(sfd, &in, sizeof(in));

    // If Successful login then go ahed
    while(in == 0){
        read(sfd, buff, sizeof(buff));
        printf("%s\n", buff);
        int t; 
        scanf("%d", &t);
        write(sfd, &t, sizeof(t));
        if(t==1)
            addCustomer(sfd);
        else if(t==2)
            delete(sfd);
        else if(t==3)
            updateCustomer(sfd);
        else if(t==4)
            search(sfd);
        else
            break;
    }

}



void balanceEnquiry(){
    int t;
    char buff[1024];
    
    read(sfd, &t, sizeof(t));
    if(t == -1){
        read(sfd, buff, sizeof(buff));       
        printf("%s\n", buff);              
    }

    read(sfd, buff, sizeof(buff));       
    printf("%s\n", buff);              
}


void withdraw(){
    
    int t;
    char buff[1024];
    double amt;

    read(sfd, buff, sizeof(buff));       // read "How much amt want to withdraw?"
    printf("%s\n", buff);              
    scanf("%lf", &amt);
    write(sfd, &amt, sizeof(amt));

    read(sfd, &t, sizeof(t));

    if(t == -1){     
        read(sfd, buff, sizeof(buff));
        printf("%s\n", buff);  
    }

    read(sfd, buff, sizeof(buff));       // read "Updated balance" or Insufficient balance
    printf("%s\n", buff);              
}


void pwd_name_Change(){
     int t;
    char buff[1024];
    char userName[64], password[64];

    read(sfd, buff, sizeof(buff));      // read "Enter username"
    printf("%s\n", buff);               
    scanf("%s", userName);
    write(sfd, userName, sizeof(userName));

    read(sfd, &t, sizeof(t));
    if(t == 0){

          // Check critical section available or not
        read(sfd, &t, sizeof(t));

        if(t == -1){     
            read(sfd, buff, sizeof(buff));
            printf("%s\n", buff);  
        }

        int tt;
        while(1){
            read(sfd, buff, sizeof(buff));      // read "Enter new username"
            printf("%s\n", buff);               
            scanf("%s", userName);
            write(sfd, userName, sizeof(userName));
            
            read(sfd, &tt, sizeof(tt));
            if(tt == 0)
                break;
            else{
                read(sfd, buff, sizeof(buff));      // read error msg "userName is already taken"
                printf("%s\n", buff);               
            }
        }

        read(sfd, buff, sizeof(buff));      // read "Enter password"
        printf("%s\n", buff);               
        scanf("%s", password);
        write(sfd, password, sizeof(password));

        printf("Details Updated Successfully.\n\n");
    }
    else{
        read(sfd, buff, sizeof(buff));          // read erroe msg "no such userName"
        printf("%s\n", buff);               
    }
}

void viewDetails(){

    char buff[1024*10];
    int t;
    read(sfd, &t, sizeof(t));       

    if(t == 0){

        read(sfd, buff, sizeof(buff));       
    
        int fd_tmp = open(".tmp", O_CREAT | O_RDWR, 0644);
        write(fd_tmp, buff, strlen(buff));
        close(fd_tmp);

        system("echo '\n------------------------------- Transaction History -------------------------------' ");    
        system("cat .tmp | column -t");       
        system("rm .tmp");   
        printf("\n\n");

    }else{
        
        read(sfd, buff, sizeof(buff));       
        printf("%s\n", buff);    

    }

}


void deposit(){
    
    int t;
    char buff[1024];
    double amt;

    read(sfd, buff, sizeof(buff));       // read "How much amt want to deposit?"
    printf("%s\n", buff);              
    scanf("%lf", &amt);
    write(sfd, &amt, sizeof(amt));

    read(sfd, &t, sizeof(t));

    if(t == -1){     
        read(sfd, buff, sizeof(buff));
        printf("%s\n", buff);  
    }

    read(sfd, buff, sizeof(buff));       // read "Updated balance"
    printf("%s\n", buff);              
}


void customerClient(){

    char buff[1024];
    int in;
    char password[MAXPW] = {0};
    char *p = password;
    FILE *fp = stdin;
    ssize_t nchr = 0;

    
    // Enter UserName & Password for Customer login and send it for authentication
    char userName[64];
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    scanf("%s", userName);
    write(sfd, userName, sizeof(userName));
    
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    getchar();
    nchr = getpasswd (&p, MAXPW, '*', fp);
    write(sfd, password, sizeof(password));
    printf("\n");

    // Get Response of Authentication
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    read(sfd, &in, sizeof(in));

    // If Successful login then go ahed
    while(in == 0){
        read(sfd, buff, sizeof(buff));
        printf("%s\n", buff);
        int t; 
        scanf("%d", &t);
        write(sfd, &t, sizeof(t));
        if(t==1)
            deposit();
        else if(t==2)
            withdraw();
        else if(t==3)
            balanceEnquiry();
        else if(t==4)
            pwd_name_Change();
        else if(t==5)
            viewDetails();
        
        else
            break;
    }
}

int main(){

    struct sockaddr_in server;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5000);

    printf("Connecting to ONLINE BANKING SYSTEM server ... \n");
    connect(sfd, (struct sockaddr*)&server, sizeof(server)); 
    
    // HomePage 
    char buff[1024];
    int in;
    read(sfd, buff, sizeof(buff));
    printf("%s\n", buff);
    scanf("%d", &in);
    write(sfd, &in, sizeof(in));

    if(in == 1)
        adminClient(sfd);
    else
        customerClient(sfd);

    close(sfd);
    return 0;
}


