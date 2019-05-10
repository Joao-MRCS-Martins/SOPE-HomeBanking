#include "logging_user.h"

void log_reply(tlv_reply_t *reply){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);
    pid_t id = getpid();

    logReply(fd, id, reply);
}

void log_request(tlv_request_t *request){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);
    pid_t id = getpid();

    logRequest(fd, id, request);
}
