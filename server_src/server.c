#include "../auxiliary_code/sope.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


int main () {
    //recieve user requests
    int fd, fd_dummy;
    char name [MAX_PASSWORD_LEN];
    int pid;
    int opcode;
    //send server responses
    int fd2;
    char fifo_path [USER_FIFO_PATH_LEN];
    char response[MAX_PASSWORD_LEN];
    
    if(mkfifo(SERVER_FIFO_PATH,RDWR_USGR) < 0)
        if(errno==EEXIST) {
            //perror(SERVER_FIFO_PATH);
            //return RC_OTHER;
        }
        else
        {
            return RC_OTHER;
        }

    printf("server checkpoint 1\n");
    
    if((fd = open(SERVER_FIFO_PATH,O_RDONLY)) == -1) {
        return RC_SRV_DOWN;
    }
    
    printf("server checkpoint 2\n");
    
    if((fd_dummy = open(SERVER_FIFO_PATH,O_WRONLY)) == -1) {
        return RC_SRV_DOWN;
    }
    
    printf("server checkpoint 3\n");
    
    //read pid of user and open fifo for comunication
    read(fd, &pid, sizeof(int));
    printf("User had pid %d\n",pid);
    sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,pid);
    fd2 = open(fifo_path,O_WRONLY);
    if(fd2 == -1) {
        return RC_USR_DOWN;
    }
    
    printf("server checkpoint 4\n");
    
    do
    {
        read(fd, &opcode, sizeof(int));
        if(opcode != 0) {
            read(fd,name, MAX_PASSWORD_LEN);
            printf("%s has opcode %d\n",name,opcode);
            sprintf(response,"user nº%d is gay",pid);
            write(fd2,response,sizeof(response));
        }
    } while (opcode != 0);

    printf("server checkpoint 5\n");

    close(fd);
    close(fd_dummy);
    if(unlink(SERVER_FIFO_PATH) < 0) {
        printf("Error when destroying FIFO '%s'\n",SERVER_FIFO_PATH);
        return RC_OTHER;
    }
    else
        printf("FIFO '%s' has been destroyed\n",SERVER_FIFO_PATH);
    
    close(fd2);
    return RC_OK;
}