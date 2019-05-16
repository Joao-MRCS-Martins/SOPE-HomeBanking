#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "process_request.h"
#include "request_queue.h"
#include "../auxiliary_code/constants.h"
#include "e_counter.h"

bool server_shutdown = false;

static pthread_t e_counters[MAX_BANK_OFFICES];

static pthread_mutex_t queue_lock;

void init_e_counters() {
    memset(&e_counters,0,sizeof(e_counters)); 
}

void* start_e_counter(void* args) {
    e_counter_t* e_counter_info = (e_counter_t*) args;
    request_queue_t* request_queue = e_counter_info->request_queue;
    // int id = e_counter_info->id; to be used later

    char fifo_path [USER_FIFO_PATH_LEN];
    int rs;
    tlv_reply_t reply;

    while (!( server_shutdown && empty_request_queue(request_queue) )) {
        request_queue_wait_for_request(request_queue);

        if (empty_request_queue(request_queue)) {
            break;
        }

        pthread_mutex_lock(&queue_lock);        

        tlv_request_t request = get_request_queue_front(request_queue);
        request_queue_pop(request_queue);

        pthread_mutex_unlock(&queue_lock);

        sprintf(fifo_path,"%s%d",USER_FIFO_PATH_PREFIX,request.value.header.pid);
        rs = open(fifo_path,O_WRONLY);
        if(rs == -1) {
            continue; //failing to communicate with user, continues listening for requests
        }

        process_request(&request,&reply);
        
        write(rs,&reply,sizeof(reply));

        close(rs);

        if (request.type == OP_SHUTDOWN && reply.value.header.ret_code == RC_OK ) {
            server_shutdown = true;
        }
        if (server_shutdown && empty_request_queue(request_queue)) {
            unlock_threads(request_queue);
        }
    }

    free(args);

    pthread_exit(NULL);
}

int create_e_counters(request_queue_t* request_queue, int n_threads) {
    pthread_mutex_init(&queue_lock,NULL);

    for (int i = 1; i <= n_threads; i++) {
         e_counter_t* new_e_counter = malloc(sizeof(e_counter_t));
         new_e_counter->id = i;
         new_e_counter->request_queue = request_queue;

        pthread_t tid;

        pthread_create(&tid,NULL,start_e_counter,(void*) new_e_counter);

        //add the thread to the array to join later
        e_counters[i-1] = tid;
    }

    return 0;
}

int wait_for_e_counters() {
   for (int i = 0; i < MAX_BANK_OFFICES; i++) {
       if (e_counters[i] == 0) {
           break;
       }

        pthread_join(e_counters[i],NULL);
    }

    return 0;
}