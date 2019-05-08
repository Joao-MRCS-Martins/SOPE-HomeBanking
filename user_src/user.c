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
#include <pthread.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[]) {
    
    if(argc != 6) {
        //show_usage();
        printf("Wrong arguments\n");
        return RC_LOGIN_FAIL;
    }

    //open log file

    //parsing input

    //send request
    int rq, opcode;
    char request[MAX_PASSWORD_LEN+WIDTH_ID+1];
    //receive answer from server
    int rs;
    char fifo_path [USER_FIFO_PATH_LEN];
    char response [MAX_PASSWORD_LEN];
    int pid = getpid();
    time_t begin;
    
    sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,pid);
    if (mkfifo(fifo_path,RDWR_USGR)<0) {
        if (errno==EEXIST) {
            //return RC_OTHER;
        }
        else {
            perror(fifo_path);
            return RC_OTHER;
        }
    }

    rq=open(SERVER_FIFO_PATH,O_WRONLY);
    if (rq == -1) {
        return RC_USR_DOWN;
    }

    write(rq,&pid,sizeof(int));

    if ((rs=open(fifo_path,O_RDONLY)) == -1) {
        return RC_USR_DOWN;
    }

    //parse request
    opcode = atoi(argv[1]);
    write(rq,&opcode,sizeof(int));
    
    bool timeout = true;
    if(opcode !=0) {
        time(&begin);
        while(difftime(time(NULL),begin) <= FIFO_TIMEOUT_SECS) {
            if(read(rs,response,sizeof(response)) > 0) {
                printf("%s\n",response);
                timeout = false;
                break;
            }
        }
    }

    close(rq);
    close(rs);
    if(unlink(fifo_path) < 0) {
        printf("Error when destroying FIFO '%s'\n",fifo_path);
        return RC_OTHER;
    }
    else
        printf("FIFO '%s' has been destroyed\n",fifo_path);

    if(timeout && opcode != 0)
        return RC_SRV_TIMEOUT;
    else
        return RC_OK;
}