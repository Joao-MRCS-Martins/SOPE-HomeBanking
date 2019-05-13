#include "request_queue.h"

request_queue_t* request_queue_init() {
    request_queue_t* queue = malloc(sizeof(request_queue_t));

    queue->front = NULL;
    queue->rear = NULL;
    sem_init(&queue->unhandeled_requests,SHARED,0);

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

    new_node->data = item;
    new_node->next = NULL;

    //increases semaphore
    sem_post(&queue->unhandeled_requests);

    //insert first node if the queue is empty
    if (empty_queue(queue)) {
        queue->front = new_node;
        queue->rear = new_node;
    }
    else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    return 0;
}


int request_queue_pop(request_queue_t* queue) {
    if (empty_queue(queue)) {
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
        return 0;
    }
} 

tlv_request_t get_request_queue_front(request_queue_t* queue) { 
    return queue->front->data;
}
 

void request_queue_delete(request_queue_t* queue) {
    while(!empty_queue(queue)) {
        queue_pop(queue);
    }
    free(queue);
}

