#include "../auxiliary_code/sope.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "authentication.h"
#include "logging_server.h"

void load_admin(bank_account_t* admin);

void process_request(tlv_request_t *request, tlv_reply_t *reply, int rq);

void create_account(tlv_request_t *req,tlv_reply_t *rep);

void shutdown(tlv_request_t *request, tlv_reply_t *reply, int rq);

void balance(tlv_request_t *request, tlv_reply_t *reply);

void transfer(tlv_request_t *request, tlv_reply_t *reply);

void clean_accounts();