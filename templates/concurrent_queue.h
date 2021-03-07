#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

struct c_queue {
	int begin, end;
	int size, max_size;
	void ** queue;
	pthread_mutex_t read_lock;
	pthread_mutex_t write_lock;
	sem_t prod, cons;
};

void push(struct c_queue*, void*);

void * pop(struct c_queue*);

int is_empty(struct c_queue*);

void queue_init(struct c_queue*, int);

void queue_destroy(struct c_queue*);