#include "../auxiliary_code/sope.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "authentication.h"
#include "logging_server.h"

void load_admin(bank_account_t* admin);

int process_request(tlv_request_t *request, tlv_reply_t *reply);