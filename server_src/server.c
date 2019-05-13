#include "server.h"
#include "server_parser.h"
#include "process_request.h"
#include "../auxiliary_code/show_info.h"
#include "request_queue.h"

static bank_account_t admin; //is it necessary?

static request_queue_t* request_queue;

int receive_requests();

int main (int argc, char *argv []) {

    if(argc != 3) {
        printf("Wrong number of arguments.\n");
        show_usage_server();
        return FAILURE;
    }

    //parse input and create admin account
    int nthr;
    

    if(input_parser(argv,&admin,&nthr) != SUCCESS) {
        show_usage_server();
        return FAILURE;
    }

    //initalize request queue
    request_queue = request_queue_init();

    //create threads (MISSING)

    //logBankOfficeOpen
    open_server(ADMIN_ACCOUNT_ID);

    //load admin into bank accounts
    load_admin(&admin);
    
    //log admin account creation 
    log_creat_acc(&admin,ADMIN_ACCOUNT_ID);


    if(mkfifo(SERVER_FIFO_PATH,RDWR_USGR) < 0) {
        if(errno==EEXIST) {
            perror(SERVER_FIFO_PATH);
            return FAILURE;
        }
        else
            return FAILURE;
    }

    int rc = receive_requests();

    if(unlink(SERVER_FIFO_PATH) < 0) {
        printf("Error when destroying FIFO '%s'\n",SERVER_FIFO_PATH);
        return FAILURE;
    }
    else
        printf("FIFO '%s' has been destroyed\n",SERVER_FIFO_PATH);
    
    close_server(ADMIN_ACCOUNT_ID);

    request_queue_delete(request_queue);

    return rc;
}

int receive_requests() {

    //receive user requests
    int rq;
    tlv_request_t request;
    //send server responses (HALFWAY)
    int rs;
    char fifo_path [USER_FIFO_PATH_LEN];
    tlv_reply_t reply;
    
    if((rq = open(SERVER_FIFO_PATH,O_RDONLY)) == -1) {
        return FAILURE;
    }    
    
    do
    {
        if(read(rq, &request, sizeof(request)) == 0) {
            usleep(500);
            continue;
        }

        //read pid of user and open fifo for comunication
        sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,request.value.header.pid);
        rs = open(fifo_path,O_WRONLY);
        if(rs == -1) {
            continue; //failing to communicate with user, continues listening for requests
        }

        process_request(&request,&reply, rq);
        
        write(rs,&reply,sizeof(reply));

        close(rs);
        
    } while (request.type != OP_SHUTDOWN || reply.value.header.ret_code != RC_OK); //needs to check for valid request

    close(rq);
    
    return SUCCESS;
}