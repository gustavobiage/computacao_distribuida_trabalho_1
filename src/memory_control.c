#include "../templates/memory_control.h"

#define NIL -1

int MEMORY_SIZE;
int SECTION_AMNT;
int SQRT_VALUE;

char *FILENAME;
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

	char cache[MAX_BUFFER_SIZE];
	setvbuf(filep, cache, _IONBF, MAX_BUFFER_SIZE);

	fseek(filep, begin, SEEK_SET);
	for (int i = begin; i < begin + length; i++) {
		lock_index = get_lock_index(i);
		if (lock_index != old_lock_index) {
			if (sem_wrt != NULL) {
				// UNLOCK
				fflush(filep);
				unlock_as_writer(sem_wrt);
			}
			// UPDATE control
			sem_wrt = &write_semaphores[lock_index];
			// LOCK
			lock_as_writer(sem_wrt);
		}
		//overhead...
		// writes 1 char at a time
		fwrite(&bytes[i-begin], sizeof(char), 1, filep);
	}
	if (sem_wrt != NULL) {
		// UNLOCK write
		fflush(filep);
		unlock_as_writer(sem_wrt);
	}
	fclose(filep);
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

	char cache[MAX_BUFFER_SIZE];
	setvbuf(filep, cache, _IONBF, MAX_BUFFER_SIZE);

	fseek(filep, begin, SEEK_SET);
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
		fread(&bytes[i - begin], sizeof(char), 1, filep);
		// bytes[i - begin] = memory[i];
	}
	if (mutex_lck != NULL && sem_wrt != NULL && readers != NULL) {
		unlock_as_reader(sem_wrt, mutex_lck, readers);
	}
	fclose(filep);
	bytes[begin + length] = '\n';

	return bytes;
}

void memory_control_init(char * server_name, int mem_size) {
	int filename_len = strlen(MEMORY_DIR) + 1 + strlen(server_name) + 5; // '/' + .mem + '\0'
	FILENAME = (char*) malloc(sizeof(char) * filename_len);
	sprintf(FILENAME, "%s/%s.mem", MEMORY_DIR, server_name);

	MEMORY_SIZE = mem_size;
	SQRT_VALUE = (int) sqrt(MEMORY_SIZE);
	SECTION_AMNT = SQRT_VALUE;

	memory = (char *) malloc(sizeof(char) * MEMORY_SIZE);
	write_semaphores = (sem_t *) malloc(sizeof(sem_t) * SECTION_AMNT);
	mutexes = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * SECTION_AMNT);
	read_cnt = (int * ) malloc(sizeof(int) * SECTION_AMNT);

	for (int i = 0; i < SECTION_AMNT; i++) {
		sem_t* semaphore = &write_semaphores[i];
		sem_init(semaphore, 0, 1);

		pthread_mutex_t* mutex = &mutexes[i];
		pthread_mutex_init(mutex, NULL);
	}

	if (fopen(FILENAME, "r+") == NULL) {
		FILE *memfp = fopen(FILENAME, "w");
		fclose(memfp);
	}
	// Set file with size
	truncate(FILENAME, MEMORY_SIZE);
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