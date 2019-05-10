#include "process_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            //validate admin
            //create account
            break;
        case OP_SHUTDOWN:
            //validate admin
            //shutdown server
            break;
        case OP_BALANCE:
            //validate client
            //check account balance
            break;
        case OP_TRANSFER:
            //validate client
            //make transfer
            break;
        default:
            //do nothing
            break;
    }
    return RC_OK;
}