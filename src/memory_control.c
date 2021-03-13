#include "../templates/memory_control.h"

#define NIL -1

int MEMORY_SIZE;
int SECTION_AMNT;
int SQRT_VALUE;

const char *FILENAME = "server_mem";
char * memory;
sem_t * write_semaphores;
pthread_mutex_t * mutexes;
pthread_t logger_thread;
int * read_cnt;
int shutdown = 0;

int get_lock_index(int index) {
	return index / SQRT_VALUE;
}

void lock_as_writer(sem_t* write_semaphore) {
	sem_wait(write_semaphore);
}

void unlock_as_writer(sem_t* write_semaphore) {
	sem_post(write_semaphore);
}

void write_data(char * bytes, int begin, int length) {
	int lock_index;
	int old_lock_index = NIL;
	sem_t * sem_wrt = NULL;

	FILE *filep = fopen(FILENAME, "r+");

	for (int i = begin; i < begin + length; i++) {
		lock_index = get_lock_index(i);
		if (lock_index != old_lock_index) {
			if (sem_wrt != NULL) {
				// UNLOCK
				unlock_as_writer(sem_wrt);
			}
			// UPDATE control
			sem_wrt = &write_semaphores[lock_index];
			// LOCK
			lock_as_writer(sem_wrt);
		}
		fseek(filep, i, begin);
		//overhead...
		// writes 1 char at a time
		fwrite(bytes++, sizeof(char), sizeof(char), filep);
		// memory[i] = bytes[i - begin]; 
	}
	if (sem_wrt != NULL) {
		// UNLOCK write
		fclose(filep);
		sem_post(sem_wrt);
	}
}

void lock_as_reader(sem_t * write_semaphore, pthread_mutex_t * mutex, int * readers) {
	// LOCK mutex
	pthread_mutex_lock(mutex);
	// increase amnt of readers
	*readers = *readers + 1;
	if (*readers == 1) {
		// LOCK write
		sem_wait(write_semaphore);
	}
	// UNLOCK mutex
	pthread_mutex_unlock(mutex);
}

void unlock_as_reader(sem_t * write_semaphore, pthread_mutex_t * mutex, int * readers) {
	// LOCK mutex
	pthread_mutex_lock(mutex);
	// subtract amount of readers
	*readers = *readers - 1;
	if (*readers == 0) {
		// UNLOCK write
		sem_post(write_semaphore);
	}
	// UNLOCK mutex
	pthread_mutex_unlock(mutex);
}

char * read_data(int begin, int length) {
	char * bytes = (char *) malloc(sizeof(char) * (begin + length) + 1);
	int lock_index;
	int old_lock_index = NIL;
	sem_t * sem_wrt = NULL;
	pthread_mutex_t * mutex_lck = NULL;
	int * readers = NULL;

	FILE *filep = fopen(FILENAME, "r+");

	for (int i = begin; i < begin + length; i++) {
		lock_index = get_lock_index(i);
		if (lock_index != old_lock_index) {
			if (mutex_lck != NULL && sem_wrt != NULL && readers != NULL) {
				// UNLOCK
				unlock_as_reader(sem_wrt, mutex_lck, readers);
			}
			// UPDATE control
			sem_wrt = &write_semaphores[lock_index];
			mutex_lck = &mutexes[lock_index];
			readers = &read_cnt[lock_index];
			// LOCK
			lock_as_reader(sem_wrt, mutex_lck, readers);
		}
		fseek(filep, i, begin);
		fread(bytes++, sizeof(char), sizeof(char), filep);
		// bytes[i - begin] = memory[i];
	}
	fclose(filep);
	bytes[begin + length] = '\n';

	return bytes;
}

void memory_control_init(int mem_size) {
	MEMORY_SIZE = mem_size;
	SQRT_VALUE = (int) sqrt(MEMORY_SIZE);
	SECTION_AMNT = SQRT_VALUE;

	memory = (char *) malloc(sizeof(char) * MEMORY_SIZE);
	write_semaphores = (sem_t *) malloc(sizeof(sem_t) * SECTION_AMNT);
	mutexes = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * SECTION_AMNT);
	read_cnt = (int * ) malloc(sizeof(int) * SECTION_AMNT);
	
	if (fopen(FILENAME, "r+") == NULL) {
		FILE *memfp = fopen(FILENAME, "w");
		// Set file with size
		fseek(memfp, MEMORY_SIZE, SEEK_SET);
		fclose(memfp);
	}

	for (int i = 0; i < SECTION_AMNT; i++) {
		sem_t* semaphore = &write_semaphores[i];
		sem_init(semaphore, 0, 1);

		pthread_mutex_t* mutex = &mutexes[i];
		pthread_mutex_init(mutex, NULL);
	}

	// memset(memory, 0, sizeof(char) * MEMORY_SIZE);
}

void memory_control_destroy() {
	shutdown = 1;

	for (int i = 0; i < SECTION_AMNT; i++) {
		sem_t* semaphore = &write_semaphores[i];
		sem_destroy(semaphore);

		pthread_mutex_t* mutex = &mutexes[i];
		pthread_mutex_destroy(mutex);
	}

	free(memory);
	free(write_semaphores);
	free(mutexes);
	free(read_cnt);
}