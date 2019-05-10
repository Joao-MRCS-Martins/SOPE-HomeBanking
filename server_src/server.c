#include "server.h"

#include "server_parser.h"
#include "process_request.h"

static bank_account_t admin;

int receive_requests();

void create_AdminAccount(char* password){
    admin.account_id = ADMIN_ACCOUNT_ID;
    admin.balance = 0;
    strcpy(admin.salt, generateSALT());
    strcpy(admin.hash, generateHASH(admin.salt, password));
}

int main (int argc, char *argv []) {

    if(argc != 3) {
        printf("Wrong arguments\n");
        //show_usage():
        return RC_LOGIN_FAIL;
    }

    //parse input and create admin bank account
    create_AdminAccount(argv[2]);


    int nthr;
    int rc;
    if((rc = input_parser(argv,&admin,&nthr)) > 0) {
        return rc;
    }

    // //recieve user requests
    // int fd, fd_dummy;
    // char name [MAX_PASSWORD_LEN];
    // int pid;
    // int opcode;
    // //send server responses
    // int fd2;
    // char fifo_path [USER_FIFO_PATH_LEN];
    // char response[MAX_PASSWORD_LEN];

    //load admin into bank accounts
    load_admin(&admin);
    
    //create threads

    //logBankOfficeOpen
    open_server(ADMIN_ACCOUNT_ID);

    //logAccountCreation

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
    //send server responses
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