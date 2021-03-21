#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_BUFFER_SIZE 100

int get_lock_index(int);

void lock_as_writer(sem_t*);

void unlock_as_writer(sem_t*);

void write_data(char *, int, int);

void lock_as_reader(sem_t*, pthread_mutex_t*, int*);

void unlock_as_reader(sem_t*, pthread_mutex_t*, int*);

char* read_data(int, int);

void memory_control_init(char*, int);

void memory_control_destroy();