#include "process_request.h"
#include "../auxiliary_code/show_info.h"
/*
shared memory block
array of bank accounts
e-counters
semaphores
*/

//CANT FORGET TO FREE MEMORY IN THE END (SHUTDOWN PART)
static bank_account_t* accounts[MAX_BANK_ACCOUNTS+1]; 


void load_admin(bank_account_t *admin) {
    //enter critical section ???
    log_sync_delay(0,ADMIN_ACCOUNT_ID,MAIN_THREAD_ID); // think bank_id is correct?
    accounts[0] = admin;
}

void create_account(tlv_request_t *req, tlv_reply_t *rep) {
    
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

        log_creat_acc(account,MAIN_THREAD_ID); // TO BE ALTERED, MUST BE THREAD ID
    }
    else {
        printf("Account and/or password incorrect.\n");              
        rep->value.header.ret_code = RC_LOGIN_FAIL;
    }
}

void shutdown(tlv_request_t *request, tlv_reply_t *reply, int rq) {
    
    reply->value.shutdown.active_offices = 0; // error value (to be confirmed)
    if(request->value.header.account_id != ADMIN_ACCOUNT_ID) {
        printf("Only admin can shutdown the system\n");
        reply->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)) {
                
        usleep(request->value.header.op_delay_ms);
        log_delay(request->value.header.op_delay_ms,MAIN_THREAD_ID); // TO BE ALTERED, MUST BE THREAD ID

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
}

void balance(tlv_request_t *request, tlv_reply_t *reply) {
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
}

void transfer(tlv_request_t *request, tlv_reply_t *reply) {
    reply->value.transfer.balance = request->value.transfer.amount; //error value (to be confirmed)

    if(request->value.header.account_id == ADMIN_ACCOUNT_ID) {
        printf("Only clients can transfer money\n");
        reply->value.header.ret_code = RC_OP_NALLOW;
    }
    else if(accounts[request->value.transfer.account_id] == 0 || accounts[request->value.header.account_id] == 0) {
        printf("Source or end account does not exist\n");
        reply->value.header.ret_code = RC_ID_NOT_FOUND;
    }
    else if(request->value.header.account_id == request->value.transfer.account_id) {
        printf("Source and end account are the same.\n");
        reply->value.header.ret_code = RC_SAME_ID;
    }
    else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
        if (accounts[request->value.header.account_id]->balance - request->value.transfer.amount <  MIN_BALANCE) {
            printf("The source account will be left with insufficient funds.\n");
            reply->value.header.ret_code =  RC_NO_FUNDS;
        }
        else if(request->value.transfer.amount + accounts[request->value.transfer.account_id]->balance > MAX_BALANCE) {
            printf("The end account will have excessive amount of money allowed.\n");
            reply->value.header.ret_code = RC_TOO_HIGH;
        }
        else {
            accounts[request->value.header.account_id]->balance -= request->value.transfer.amount;
            accounts[request->value.transfer.account_id]->balance += request->value.transfer.amount;
            reply->value.transfer.balance = accounts[request->value.header.account_id]->balance;
        }
    }
    else {
        printf("Account and/or password incorrect.\n");
        reply->value.header.ret_code = RC_LOGIN_FAIL;   
    }


}

void process_request(tlv_request_t *request, tlv_reply_t *reply, int rq) {

    show_request(*request);
    log_request(request, MAIN_THREAD_ID); // TO BE ALTERED, MUST BE THREAD ID
    
    reply->type = request->type;
    reply->value.header.account_id = request->value.header.account_id;
    
    //enter critical section
    usleep(request->value.header.op_delay_ms);
    log_sync_delay(request->value.header.op_delay_ms,request->value.header.account_id,MAIN_THREAD_ID); // TO BE ALTERED, MUST BE THREAD ID
    
    switch(request->type) {
        case OP_CREATE_ACCOUNT:
            create_account(request,reply);
            break;

        case OP_SHUTDOWN:
            shutdown(request,reply,rq);
            break;

        case OP_BALANCE:
            balance(request, reply);
            break;

        case OP_TRANSFER:
            transfer(request, reply);
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
    
    log_reply(reply, MAIN_THREAD_ID); // TO BE ALTERED, MUST BE THREAD ID
}

void clean_accounts() {
    for(int i =0; i < MAX_BANK_ACCOUNTS+1;i++) {
        if(accounts[i] != 0) {
            free(accounts[i]);
        }
    } 
}