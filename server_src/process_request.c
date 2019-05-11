#include "process_request.h"
/*
shared memory block
array of bank accounts
e-counters
semaphores
*/
static bank_account_t* accounts[MAX_BANK_ACCOUNTS+1];

void load_admin(bank_account_t *admin) {
    accounts[0] = admin;
}

int process_request(tlv_request_t *request, tlv_reply_t *reply) {
    switch(request->type) {
        case OP_CREATE_ACCOUNT:
            reply->type = request->type;
            reply->value.header.account_id = request->value.header.account_id;

            if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                log_request(request, request->value.header.account_id);

                bank_account_t account;
                account.account_id = request->value.create.account_id;
                account.balance = request->value.create.balance;
                strcpy(account.salt, generateSALT());
                strcpy(account.hash, generateHASH(account.salt, request->value.create.password));
                
                accounts[account.account_id] = &account;

                reply->value.header.ret_code = RC_OK;
            }
            else
            {
                reply->value.header.ret_code = RC_LOGIN_FAIL;
            }

            log_reply(reply, request->value.header.account_id);

            break;
        case OP_SHUTDOWN:
            if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
            }
            else
            {
                
            }
            break;
        case OP_BALANCE:
            if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                
            }
            else
            {
                
            }
            break;
        case OP_TRANSFER:
            if(checkPassword(accounts[request->value.header.account_id], request->value.header.password)){
                            //make transfer
            }
            else
            {
                
            }
            break;
        default:
            //do nothing
            break;
    }
    return RC_OK;
}