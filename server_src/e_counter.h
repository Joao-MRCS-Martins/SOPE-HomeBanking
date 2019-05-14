#ifndef _E_COUNTER_H_
#define _E_COUNTER_H_

void init_e_counters(pthread_t main_tid);

void* start_e_counter(void* args);

int create_e_counter(request_queue_t* request_queue);

int create_e_counters(request_queue_t* request_queue, int n_threads);

#endif
