#include "../templates/concurrent_queue.h"

void push(struct c_queue * queue, void * element) {
	sem_wait(&queue->prod);
	pthread_mutex_lock(&queue->write_lock);
	int index = queue->end;
	queue->queue[index++] = element;
	index = index % queue->max_size;
	queue->end = index;
	queue->size++;
	pthread_mutex_unlock(&queue->write_lock);
	sem_post(&queue->cons);
}

void * pop(struct c_queue * queue) {
	sem_wait(&queue->cons);
	pthread_mutex_lock(&queue->read_lock);
	int index = queue->begin;
	void * element = queue->queue[index++];
	index = index % queue->max_size;
	queue->begin = index;
	queue->size--;
	pthread_mutex_unlock(&queue->read_lock);
	sem_post(&queue->prod);
	return element;
}

void queue_init(struct c_queue * queue, int size) {
	queue->max_size = size;
	queue->size = 0;
	queue->begin = queue->end = 0;
	sem_init(&queue->prod, 0, size);
	sem_init(&queue->cons, 0, 0);
	queue->queue = (void **) malloc(sizeof(void*) * size);
	pthread_mutex_init(&queue->read_lock, NULL);
	pthread_mutex_init(&queue->write_lock, NULL);
}

void queue_destroy(struct c_queue * queue) {
	free(queue->queue);
	sem_destroy(&queue->cons);
	sem_destroy(&queue->prod);
	pthread_mutex_destroy(&queue->read_lock);
	pthread_mutex_destroy(&queue->write_lock);
}