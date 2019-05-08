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


int main(int argc, char *argv[]) {
    //send request
    int fd, opcode;
    char name[MAX_PASSWORD_LEN];
    char request[MAX_PASSWORD_LEN+WIDTH_ID+1];
    //receive answer from server
    int fd2, fd2_dummy;
    char fifo_path [USER_FIFO_PATH_LEN];
    char response [MAX_PASSWORD_LEN];
    int pid = getpid();
    
    if (argc!=2 && argc!=3) {
        printf("Usage: user <opcode> <username> OR user 0\n");
        exit(1);
    }

    printf("user checkpoint 1\n");

    fd=open(SERVER_FIFO_PATH,O_WRONLY);
    if (fd == -1) {
        return RC_SRV_DOWN;
    }

    write(fd,&pid,sizeof(int));

    printf("user checkpoint 2\n");
    
    sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,pid);
    if (mkfifo(fifo_path,RDWR_USGR)<0)
        if (errno==EEXIST) {
            perror(fifo_path);
            return RC_OTHER;
        }
        else {
            return RC_OTHER;
        } 
    
    printf("user checkpoint 3\n");

    if ((fd2=open(fifo_path,O_RDONLY)) == -1) {
        return RC_USR_DOWN;
    }
    if ((fd2_dummy=open(fifo_path,O_WRONLY)) ==-1) {
        return RC_USR_DOWN;
    }

    printf("user checkpoint 4\n");

    opcode=atoi(argv[1]);
    write(fd,&opcode,sizeof(int));
    if (opcode!=0) {
        write(fd,argv[2],strlen(argv[2])+1);
    }

    while(opcode !=0) {
        read(STDIN_FILENO,request, sizeof(request));
        opcode = atoi(strtok(request," "));
        write(fd,&opcode,sizeof(int));
        if(opcode == 0)
            break;
        strcat(name,strtok(NULL,""));
        write(fd,name,sizeof(name));
        read(fd2,response,sizeof(response));
        printf("%s\n",response);
        
    }

    printf("user checkpoint 5\n");
    close(fd2);
    close(fd2_dummy);
    if(unlink(fifo_path) < 0) {
        printf("Error when destroying FIFO '%s'\n",fifo_path);
        return RC_OTHER;
    }
    else
        printf("FIFO '%s' has been destroyed\n",fifo_path);
    close(fd);
    return RC_OK;
}