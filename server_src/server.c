#include "server.h"
#include "server_parser.h"
#include "process_request.h"
#include "../auxiliary_code/show_info.h"

static bank_account_t admin;

int receive_requests();

int main (int argc, char *argv []) {

    if(argc != 3) {
        printf("Wrong number of arguments.\n");
        show_usage_server();
        return RC_LOGIN_FAIL;
    }

    //parse input and create admin account
    int nthr;
    int rc;
    if((rc = input_parser(argv,&admin,&nthr)) > 0) {
        show_usage_server();
        return rc;
    }

    //log admin account creation (MISSING)

    //load admin into bank accounts
    load_admin(&admin);
    
    //create threads (MISSING)

    //logBankOfficeOpen
    open_server(ADMIN_ACCOUNT_ID);


    if(mkfifo(SERVER_FIFO_PATH,RDWR_USGR) < 0) {
        if(errno==EEXIST) {
            //perror(SERVER_FIFO_PATH);
            //return RC_OTHER;
        }
        else
            return RC_OTHER;
    }

    rc = receive_requests();

    if(unlink(SERVER_FIFO_PATH) < 0) {
        printf("Error when destroying FIFO '%s'\n",SERVER_FIFO_PATH);
        return RC_OTHER;
    }
    else
        printf("FIFO '%s' has been destroyed\n",SERVER_FIFO_PATH);
    
    close_server(ADMIN_ACCOUNT_ID);

    return rc;
}

int receive_requests() {

    //receive user requests
    int rq;
    tlv_request_t request;
    //send server responses (HALFWAY)
    int rs;
    char fifo_path [USER_FIFO_PATH_LEN];
    //tlv_reply_t reply;
    char response[MAX_BUFFER];
    
    if((rq = open(SERVER_FIFO_PATH,O_RDONLY)) == -1) {
        return RC_SRV_DOWN;
    }    
    
    do
    {
        if(read(rq, &request, sizeof(request)) == 0) {
            usleep(500);
            continue;
        }

        show_request(request);
        //read pid of user and open fifo for comunication
        sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,request.value.header.pid);
        rs = open(fifo_path,O_WRONLY);
        if(rs == -1) {
            return RC_USR_DOWN;
        }

        printf("User %d has opcode %d\n",request.value.header.pid,request.type);
       
        sprintf(response,"User nº%d has a weak pass: %s",request.value.header.pid,request.value.header.password);
        write(rs,response,sizeof(response));

        close(rs);
    } while (request.type != OP_SHUTDOWN);

    close(rq);
    
    return RC_OK;
}