#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "logging_user.h"
#include "user_parser.h"
#include "../auxiliary_code/show_info.h"

int main(int argc, char *argv[]) {
    
    if(argc != 6) {
        printf("Wrong number of arguments.\n");
        show_usage_user();
        return RC_LOGIN_FAIL;
    }

    //parsing request
    int rq;
    tlv_request_t request;
    int rc;
    if ((rc = input_parser(argv,&request))  > 0) {
        show_usage_user();
        return rc;
    }
    
    log_request(&request);

    //receive answer from server
    int rs;
    char fifo_path [USER_FIFO_PATH_LEN];
    tlv_reply_t reply;
    time_t begin;
    
    sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,request.value.header.pid);
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

    write(rq,&request,sizeof(request));

    
    if ((rs=open(fifo_path,O_RDONLY|O_NONBLOCK)) == -1) {
        return RC_USR_DOWN;
    }
    
    bool timeout = true;
    time(&begin);
    while(difftime(time(NULL),begin) <= FIFO_TIMEOUT_SECS) {
        if(read(rs,&reply,sizeof(reply)) > 0) {
            log_reply(&reply);
            show_reply(reply);
            timeout = false;
            break;
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

    if(timeout) {
        printf("Timeout on reply.\n");
        return RC_SRV_TIMEOUT;
    }
    else
        return RC_OK;
}