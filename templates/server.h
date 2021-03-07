#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "client_server.h"
#include "memory_control.h"
#include "concurrent_queue.h"

#define SECONDS 1000
#define MINUTS 60*SECONDS
#define HOURS 60*MINUTS
#define DAYS 24*HOURS

#define LOCAL_HOST "127.0.0.1"
#define DEFAULT_PORT 8081
#define DEFAULT_BACKLOG 10
#define DEFAULT_MEM_SIZE 10000

struct thread_par {
	int client_sockfd;
	pthread_t* self_pointer;
	struct c_queue thread_queue;
};

void evaluate_args(int argc, char ** argv);

void server_init();

void server_destroy();