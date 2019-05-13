#include "logging_user.h"

void log_reply(tlv_reply_t *reply){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);

    logReply(fd, reply->value.header.account_id, reply);

    close(fd);
    fclose(f);
}

void log_request(tlv_request_t *request){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);

    logRequest(fd, request->value.header.account_id, request);
    close(fd);
    fclose(f);
}
