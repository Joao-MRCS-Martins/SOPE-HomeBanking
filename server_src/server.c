#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char *argv []) {

    if(argc != 3) {
        printf("Wrong arguments\n");
        //show_usage():
        return RC_LOGIN_FAIL;
    }

    //open log file

    //parsing input

    //create threads

    //create admin account with password

    //logBankOfficeOpen

    //logAccountCreation

    if(mkfifo(SERVER_FIFO_PATH,RDWR_USGR) < 0) {
        if(errno==EEXIST) {
            //perror(SERVER_FIFO_PATH);
            //return RC_OTHER;
        }
        else
            return RC_OTHER;
    }

    int rc = handle_requests();

    if(unlink(SERVER_FIFO_PATH) < 0) {
        printf("Error when destroying FIFO '%s'\n",SERVER_FIFO_PATH);
        return RC_OTHER;
    }
    else
        printf("FIFO '%s' has been destroyed\n",SERVER_FIFO_PATH);
    
    return rc;
}

int handle_requests() {

    //receive user requests
    int rq;
    int pid;
    int opcode;
    //send server responses
    int rs;
    char fifo_path [USER_FIFO_PATH_LEN];
    char response[MAX_PASSWORD_LEN];
    
    if((rq = open(SERVER_FIFO_PATH,O_RDONLY)) == -1) {
        return RC_SRV_DOWN;
    }    
    
    do
    {
        if(read(rq, &pid, sizeof(int)) == 0) {
            usleep(500);
            continue;
        }
        if(pid == 0) {
            continue;
        }
        //read pid of user and open fifo for comunication
        sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,pid);
        rs = open(fifo_path,O_WRONLY);
        if(rs == -1) {
            return RC_USR_DOWN;
        }
        
        read(rq, &opcode, sizeof(int));
        if(opcode != 0) {
            printf("User %d has opcode %d\n",pid,opcode);
            sprintf(response,"User nº%d is gay",pid);
            write(rs,response,sizeof(response));
        }
        close(rs);
    } while (opcode != 0);

    close(rq);
    return RC_OK;
}