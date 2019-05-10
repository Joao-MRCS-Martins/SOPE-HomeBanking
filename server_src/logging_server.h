#include "../auxiliary_code/sope.h"
#include <string.h>
#include <stdlib.h>

extern struct bank_account admin;

void open_server(int bank_id);
void close_server(int bank_id);
void log_reply(tlv_reply_t *reply, int bank_id);
void log_request(tlv_request_t *request, int bank_id);
