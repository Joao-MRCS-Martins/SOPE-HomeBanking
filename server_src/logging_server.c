#include "logging_server.h"

void open_office(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeOpen(fd, bank_id, tid);

    close(fd);
    fclose(f);
}

void close_office(int bank_id){
    FILE* f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);
    pthread_t tid = pthread_self();

    logBankOfficeClose(fd, bank_id, tid);

    close(fd);
    fclose(f);
}

void log_reply(tlv_reply_t *reply, int bank_id){
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logReply(fd, bank_id, reply);
    
    close(fd);
    fclose(f);
}

void log_request(tlv_request_t *request, int bank_id){
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logRequest(fd, bank_id, request);
    
    close(fd);
    fclose(f);
}

void log_creat_acc(bank_account_t *account, int bank_id) {
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logAccountCreation(fd, bank_id, account);
    fclose(f);
}

void log_delay(uint32_t op_delay, int bank_id) {
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logDelay(fd, bank_id, op_delay);
    fclose(f);
}

void log_sync_delay(uint32_t op_delay,int acc_id,int bank_id) {
    FILE * f = fopen(SERVER_LOGFILE, "a");
    int fd = fileno(f);

    logSyncDelay(fd, bank_id, acc_id,op_delay);
    fclose(f);
}
