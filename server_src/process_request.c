#include <fcntl.h>
#include <pthread.h>
#include "process_request.h"
#include "../auxiliary_code/show_info.h"
//#include <unistd.h>


static bank_account_t* accounts[MAX_BANK_ACCOUNTS+1]; 

static pthread_mutex_t account_lock[MAX_BANK_ACCOUNTS+1];


void load_admin(bank_account_t *admin) {
    //enter critical section ???
    log_sync_delay(0,ADMIN_ACCOUNT_ID,MAIN_THREAD_ID);
    accounts[0] = admin;

    //inialize account_lock mutex array
    for (int i = 0; i < MAX_BANK_ACCOUNTS+1; i++) {
        pthread_mutex_init(&account_lock[i],NULL);
    }
}

void create_account(tlv_request_t *req, tlv_reply_t *rep, int id) {

    pthread_mutex_lock(&account_lock[req->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,req->value.header.account_id);
    
    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id); 
    
    pthread_mutex_lock(&account_lock[req->value.create.account_id]);
    log_sync(id,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,req->value.create.account_id);
    
    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id);
    
    if(req->value.header.account_id != ADMIN_ACCOUNT_ID) {
        printf("Only admin can create accounts\n");
        rep->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(checkPassword(accounts[req->value.header.account_id], req->value.header.password)){
        if(accounts[req->value.create.account_id] != 0) {
            printf("Account already exists\n");
            rep->value.header.ret_code = RC_ID_IN_USE;
            return;
        }
        
        bank_account_t *account = malloc(sizeof(bank_account_t));
        memset(account,0,sizeof(bank_account_t));

        account->account_id = req->value.create.account_id;
        account->balance = req->value.create.balance;
        
        generateSALT(account->salt);
        generateHASH(account->salt, req->value.create.password,account->hash);
        accounts[account->account_id] = account;
        rep->value.header.ret_code = RC_OK;

        log_creat_acc(account,id); 
    }
    else {
        printf("Account and/or password incorrect.\n");              
        rep->value.header.ret_code = RC_LOGIN_FAIL;
    }

    pthread_mutex_unlock(&account_lock[req->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,req->value.header.account_id);

    pthread_mutex_unlock(&account_lock[req->value.create.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,req->value.create.account_id);
}

void shutdown(tlv_request_t *request, tlv_reply_t *reply, int id) {
    pthread_mutex_lock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);

    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id);

    reply->value.shutdown.active_offices = 0; // error value 
    if(request->value.header.account_id != ADMIN_ACCOUNT_ID) {
        printf("Only admin can shutdown the system\n");
        reply->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)) {

        int rq = open(SERVER_FIFO_PATH,O_RDONLY);

        //operation delay right before closing fifo for writing
        usleep(request->value.header.op_delay_ms*THOUSAND);
        log_delay(request->value.header.op_delay_ms,id);
        
        if(fchmod(rq, READ_ALL) == 0) {
            reply->value.shutdown.active_offices = 3; // wrong value - must be active threads number
            reply->value.header.ret_code =  RC_OK;
        } 
        else {
            reply->value.header.ret_code = RC_OTHER;
        }
    }
    else {
        printf("Account and/or password incorrect.\n");
        reply->value.header.ret_code = RC_LOGIN_FAIL;
    }

    pthread_mutex_unlock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);
}

void balance(tlv_request_t *request, tlv_reply_t *reply, int id) {
    pthread_mutex_lock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);

    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id);

    reply->value.balance.balance = 0; //error value (to be confirmed)
    
    if(request->value.header.account_id == ADMIN_ACCOUNT_ID) {
        printf("Only clients can check account balance\n");
        reply->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(accounts[request->value.header.account_id] == 0) {
        printf("Account not found\n");
        reply->value.header.ret_code = RC_ID_NOT_FOUND;
    }
    else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)) {
        reply->value.balance.balance = accounts[request->value.header.account_id]->balance;
        reply->value.header.ret_code = RC_OK;
    }
    else {
        printf("Account and/or password incorrect.\n");
        reply->value.header.ret_code = RC_LOGIN_FAIL;
    }

    pthread_mutex_unlock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);
}

void transfer(tlv_request_t *request, tlv_reply_t *reply, int id) {
    pthread_mutex_lock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);

    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id);

    pthread_mutex_lock(&account_lock[request->value.transfer.account_id]);
    log_sync(id,SYNC_OP_MUTEX_LOCK,SYNC_ROLE_ACCOUNT,request->value.transfer.account_id);

    usleep(request->value.header.op_delay_ms*THOUSAND); 
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,id);

    if(request->value.header.account_id == ADMIN_ACCOUNT_ID) {
        printf("Only clients can transfer money\n");
        reply->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(accounts[request->value.header.account_id] == 0) {
        printf("Source account does not exist\n");
        reply->value.header.ret_code = RC_ID_NOT_FOUND;
        reply->value.transfer.balance = 0;
        return;
    }
    else if(accounts[request->value.transfer.account_id] == 0) {
        printf("Destination account does not exist\n");
        reply->value.header.ret_code = RC_ID_NOT_FOUND;
    }
    else if(request->value.header.account_id == request->value.transfer.account_id) {
        printf("Source and end account are the same.\n");
        reply->value.header.ret_code = RC_SAME_ID;
    }
    else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
       
        if (accounts[request->value.header.account_id]->balance  <  (MIN_BALANCE + request->value.transfer.amount)) {
            printf("The source account will be left with insufficient funds.\n");
            reply->value.header.ret_code =  RC_NO_FUNDS;
        }
        else if((request->value.transfer.amount + accounts[request->value.transfer.account_id]->balance) > MAX_BALANCE) {
            printf("The end account will have excessive amount of money allowed.\n");
            reply->value.header.ret_code = RC_TOO_HIGH;
        }
        else {
            accounts[request->value.header.account_id]->balance -= request->value.transfer.amount;
            accounts[request->value.transfer.account_id]->balance += request->value.transfer.amount;
            reply->value.transfer.balance = accounts[request->value.header.account_id]->balance;
        }

        return;
    }
    else {
        printf("Account and/or password incorrect.\n");
        reply->value.header.ret_code = RC_LOGIN_FAIL;   
    }

    reply->value.transfer.balance = accounts[request->value.header.account_id]->balance; //error value 

    pthread_mutex_unlock(&account_lock[request->value.header.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request->value.header.account_id);

    pthread_mutex_unlock(&account_lock[request->value.transfer.account_id]);
    log_sync(id,SYNC_OP_MUTEX_UNLOCK,SYNC_ROLE_ACCOUNT,request->value.transfer.account_id);

}

void process_request(tlv_request_t *request, tlv_reply_t *reply, int id) {

    show_request(*request);
    log_request(request, id); 
    
    reply->type = request->type;
    reply->value.header.account_id = request->value.header.account_id;
    
    
    switch(request->type) {
        case OP_CREATE_ACCOUNT:
            create_account(request,reply,id);
            break;

        case OP_SHUTDOWN:
            shutdown(request,reply, id);
            break;

        case OP_BALANCE:
            balance(request, reply, id);
            break;

        case OP_TRANSFER:
            transfer(request, reply, id);
            break;
            
        default:
            reply->value.header.ret_code = RC_OTHER;   
            break;
    }
    
    if(reply->type == OP_CREATE_ACCOUNT || reply->value.header.ret_code != RC_OK) {
        reply->length = sizeof(reply->value.header);
    }
    else {
        reply->length = sizeof(reply->value);
    }
    
    log_reply(reply, MAIN_THREAD_ID);
}

void clean_accounts() {
    for(int i =0; i < MAX_BANK_ACCOUNTS+1;i++) {
        if(accounts[i] != 0) {
            free(accounts[i]);
        }
        pthread_mutex_destroy(&account_lock[i]);
    } 
}