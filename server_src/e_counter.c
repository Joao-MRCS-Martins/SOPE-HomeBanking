#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "process_request.h"
#include "request_queue.h"
#include "../auxiliary_code/constants.h"
#include "e_counter.h"

bool server_shutdown = false;

static pthread_mutex_t queue_lock;

void* start_e_counter(void* args) {
    printf("thread started\n");
    e_counter_t* e_counter_info = (e_counter_t*) args;
    request_queue_t* request_queue = e_counter_info->request_queue;
    int id = e_counter_info->id;

    char fifo_path [USER_FIFO_PATH_LEN];
    int rs;
    tlv_reply_t reply;

    while (!server_shutdown) {
        request_queue_wait_for_request(request_queue);

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

    }

<<<<<<< HEAD
    free(args);

    pthread_exit(NULL);
=======
    //close_office(e_counters[gettid()]); TO FIX
    pthread_exit(NULL);
}

int create_e_counter(request_queue_t* request_queue) {
    pthread_t tid;

    pthread_create(&tid,NULL,start_e_counter,(void*) request_queue);
    
    for (int i = 0; i < MAX_BANK_OFFICES; i++) {
        if (e_counters[i] != 0) {
            e_counters[i] = tid; // not pthread_t but int (office ID)
        }
    }

    return 0;
>>>>>>> 77888f878bae0668a348f5f06961ca0e1c6aa5ad
}

int create_e_counters(request_queue_t* request_queue, int n_threads) {
    pthread_mutex_init(&queue_lock,NULL);

    for (int i = 1; i <= n_threads; i++) {
<<<<<<< HEAD
         e_counter_t* new_e_counter = malloc(sizeof(e_counter_t));
         new_e_counter->id = i;
         new_e_counter->request_queue = request_queue;

        create_e_counter(new_e_counter);
=======

        create_e_counter(request_queue);
        printf("SOMETHING\n");
        open_office(i);
>>>>>>> 77888f878bae0668a348f5f06961ca0e1c6aa5ad
    }

    return 0;
}