#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include "structures.h"

int main(){

    int fd_adm = open("./db/Admin.txt", O_CREAT | O_RDWR, 0644);

    struct Admin adm;
    strcpy(adm.userName, "root");
    strcpy(adm.password, "root");
    write(fd_adm, &adm, sizeof(adm));
    
    strcpy(adm.userName, "parth");
    strcpy(adm.password, "parth");
    write(fd_adm, &adm, sizeof(adm));

    close(fd_adm);

    return 0;
}