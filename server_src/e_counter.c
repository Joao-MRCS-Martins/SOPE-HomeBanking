#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "process_request.h"
#include "request_queue.h"
#include "../auxiliary_code/constants.h"

static pthread_t e_counters[MAX_BANK_OFFICES];

bool server_shutdown = false;

static pthread_mutex_t queue_lock;

void init_e_counters(pthread_t main_tid) {
    memset(&e_counters,0,sizeof(e_counters));
    e_counters[0] = main_tid;
}

void* start_e_counter(void* args) {
    printf("thread started\n");
    request_queue_t* request_queue = (request_queue_t*) args;

    char fifo_path [USER_FIFO_PATH_LEN];
    int rs;
    tlv_reply_t reply;

    while (!server_shutdown) {
        printf("blocked\n");
        request_queue_wait_for_request(request_queue);
        printf("not blocked\n");

        pthread_mutex_lock(&queue_lock);        

        tlv_request_t request = get_request_queue_front(request_queue);
        request_queue_pop(request_queue);
        printf("poped\n");

        pthread_mutex_unlock(&queue_lock);

        sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,request.value.header.pid);
        rs = open(fifo_path,O_WRONLY);
        if(rs == -1) {
            continue; //failing to communicate with user, continues listening for requests
        }

        process_request(&request,&reply);
        
        write(rs,&reply,sizeof(reply));

        close(rs);

        printf("THREAD: written\n");

        if (request.type == OP_SHUTDOWN && reply.value.header.ret_code == RC_OK ) {
            server_shutdown = true;
        }

        printf("thread: fin\n");
    }

    printf("exited\n");
    pthread_exit(NULL);
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

int create_e_counters(request_queue_t* request_queue, int n_threads) {
    pthread_mutex_init(&queue_lock,NULL);

    for (int i = 0; i < n_threads; i++) {
        if (i >= MAX_BANK_OFFICES) {
            break;
        }

        create_e_counter(request_queue);
    }

    return 0;
}