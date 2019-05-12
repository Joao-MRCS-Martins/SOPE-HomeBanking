#include "../auxiliary_code/sope.h"
#include <string.h>
#include <stdlib.h>

void open_server(int bank_id);
void close_server(int bank_id);
void log_reply(tlv_reply_t *reply, int bank_id);
void log_request(tlv_request_t *request, int bank_id);
void log_creat_acc(bank_account_t *account, int bank_id);
void log_delay(uint32_t op_delay, int bank_id);
void log_sync_delay(uint32_t op_delay,int acc_id,int bank_id);
