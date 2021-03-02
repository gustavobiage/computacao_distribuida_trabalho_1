#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// #include "memory_control.h"
#include "client_server.h"
#include "memory_control.h"

#define SECONDS 1000
#define MINUTS 60*SECONDS
#define HOURS 60*MINUTS
#define DAYS 24*HOURS

// #define LOCAL_HOST "127.0.0.1"
#define LOCAL_HOST 127
#define DEFAULT_PORT 8081
#define DEFAULT_THREAD_LIMIT 10
#define DEFAULT_REGISTER_LIMIT 4
#define DEFAULT_MEM_SIZE 10000

struct thread_par {
	int client_sockfd;
	struct mem_range range;
};

void evaluate_args(int argc, char ** argv);

void server_init();

void server_destroy();