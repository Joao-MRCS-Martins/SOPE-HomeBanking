#include <pthread.h>
#include <semaphore.h>
#include "server.h"
#include "request_queue.h"
#include "../auxiliary_code/constants.h"

static pthread_t e_counters[MAX_BANK_OFFICES];

void init_e_counters(pthread_t main_tid) {
    memset(&e_counters,0,sizeof(e_counters));
    e_counters[0] = main_tid;
}

void* start_e_counter(void* args) {
    request_queue_t* request_queue = (request_queue_t*) args;

    sem_wait(&request_queue->unhandeled_requests);

    
}

int create_e_counter(request_queue_t* request_queue) {
    pthread_t tid;

    pthread_create(&tid,NULL,start_e_counter,(void*) request_queue);
    
    for (int i = 0; i < MAX_BANK_OFFICES; i++) {
        if (e_counters[i] != 0) {
            e_counters[i] = tid;
        }
    }

    return 0;
}