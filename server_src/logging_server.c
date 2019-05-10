#include "logging_server.h"

void open_server(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "w");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeOpen(fd, bank_id, tid);

    close(fd);
}

void close_server(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeClose(fd, bank_id, tid);

    close(fd);
}

void log_reply(tlv_reply_t *reply, int bank_id){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);

    logReply(fd, bank_id, reply);
}

void log_request(tlv_request_t *request, int bank_id){
    FILE * f = fopen(USER_LOGFILE, "a");
    int fd = fileno(f);

    logRequest(fd, bank_id, request);
}
