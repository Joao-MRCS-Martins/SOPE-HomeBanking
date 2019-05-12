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
    accounts[0] = admin;
}

int process_request(tlv_request_t *request, tlv_reply_t *reply) {

    show_request(*request);
    log_request(request, request->value.header.account_id);
    
    reply->type = request->type;
    reply->value.header.account_id = request->value.header.account_id;
    switch(request->type) {
        case OP_CREATE_ACCOUNT:
           if(request->value.header.account_id != ADMIN_ACCOUNT_ID) {
                printf("Only admin can create accounts\n");
                reply->value.header.ret_code = RC_OP_NALLOW;
            }
            else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                reply->value.header.ret_code = create_account(request);
                //show_account(*accounts[request->value.create.account_id]);
            }
            else
            {
                reply->value.header.ret_code = RC_LOGIN_FAIL;
            }


            break;
        case OP_SHUTDOWN:
            if(request->value.header.account_id != ADMIN_ACCOUNT_ID) {
                printf("Only admin can shutdown the system\n");
                reply->value.header.ret_code = RC_OP_NALLOW;
            }
            else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
                reply->value.header.ret_code = RC_OK;
                //reply->value.header.ret_code = shutdown();
            }
            else
            {
                reply->value.header.ret_code = RC_LOGIN_FAIL;
            }
            break;
        case OP_BALANCE:
            if(request->value.header.account_id == ADMIN_ACCOUNT_ID) {
                printf("Only clients can check account balance\n");
                reply->value.header.ret_code = RC_OP_NALLOW;
            }
            else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
                //reply->value.header.ret_code = balance();
            }
            else
            {
                
                reply->value.header.ret_code = RC_LOGIN_FAIL;
            }
            break;
        case OP_TRANSFER:
            if(request->value.header.account_id == ADMIN_ACCOUNT_ID) {
                printf("Only clients can transfer money\n");
                reply->value.header.ret_code = RC_OP_NALLOW;
            }
            else if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
                //reply->value.header.ret_code = transfer();

            }
            else
            {

                reply->value.header.ret_code = RC_LOGIN_FAIL;   
            }
            break;
        default:
            reply->value.header.ret_code = RC_OTHER;   
            break;
    }
    
    if(request->type == OP_CREATE_ACCOUNT) {
        reply->length = sizeof(reply->value.header);
    }
    else {
        reply->length = sizeof(reply->value);
    }
    
    log_reply(reply, request->value.header.account_id);
    
    return RC_OK;
}

int create_account(tlv_request_t *req) {
    bank_account_t *account = malloc(sizeof(bank_account_t));
    
    if(accounts[req->value.create.account_id] != 0) {
        printf("ACCOUNT ALREADY EXISTS\n");
        return RC_ID_IN_USE;
    }

    account->account_id = req->value.create.account_id;
    account->balance = req->value.create.balance;
    strcpy(account->salt, generateSALT());
    strcpy(account->hash, generateHASH(account->salt, req->value.create.password));
    accounts[account->account_id] = account;

    return RC_OK;
}