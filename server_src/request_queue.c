#include "request_queue.h"
#include <stdlib.h>

request_queue_t* request_queue_init(int request_slots) {
    request_queue_t* queue = malloc(sizeof(request_queue_t));

    queue->front = NULL;
    queue->rear = NULL;
    queue->thread_number = request_slots; 

    sem_init(&queue->request_slots,SHARED,request_slots);
    sem_init(&queue->requests_waiting,SHARED,0);

    return queue;
}

bool empty_request_queue(request_queue_t* queue) {
    if (queue->front == NULL && queue->rear == NULL) {
        return true;
    }
    else {
        return false;
    }
}

int request_queue_push(request_queue_t* queue, tlv_request_t item) {
    node_t* new_node = malloc(sizeof(node_t));

    if (new_node == NULL) {
        return 1;
    }

    //check if there are slots available (and decreses them)
    sem_wait(&queue->request_slots);

    new_node->data = item;
    new_node->next = NULL;

    //insert first node if the queue is empty
    if (empty_request_queue(queue)) {
        queue->front = new_node;
        queue->rear = new_node;
    }
    else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    //increase requests waiting for handling
    sem_post(&queue->requests_waiting);

    return 0;
}

//poping creates a new slot 
int request_queue_pop(request_queue_t* queue) {
    if (empty_request_queue(queue)) {
        return 1;
    }
    else {
        if (queue->front == queue->rear) { //if there is only one element
            node_t* delete = queue->front;
            queue->front = NULL;
            queue->rear = NULL;
            free(delete);
        }
        else {
            node_t* delete = queue->front;
            queue->front = queue->front->next;
            free(delete);
        }

        sem_post(&queue->request_slots);

        return 0;
    }
} 

tlv_request_t get_request_queue_front(request_queue_t* queue) { 
    return queue->front->data;
}
 

void request_queue_delete(request_queue_t* queue) {
    while(!empty_request_queue(queue)) {
        request_queue_pop(queue);
    }
    free(queue);
}

void request_queue_wait_for_request(request_queue_t* queue) {
    sem_wait(&queue->requests_waiting);
}


void unlock_threads(request_queue_t* queue) {
    for (int i = 0; i < queue->thread_number; i++) {
        sem_post(&queue->requests_waiting);
    }
}
