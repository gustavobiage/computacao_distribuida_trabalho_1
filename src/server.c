#include "../template/server.h"

int SERVER_IP;
int SERVER_PORT;
int MAIN_SERVER;
int MEMORY_SIZE;
int THREAD_LIMIT;
int REGISTER_LIMIT;

pthread_t * threads;
char * used_thread;
int register_cnt;
int thread_pointer;

pthread_mutex_t mutex;

pthread_t * get_free_thread() {
	if (used_thread[thread_pointer]) {
		pthread_join(threads[thread_pointer], NULL);
	}
	used_thread[thread_pointer] = 1;
	return &threads[thread_pointer++];
}

void self_register() {
	struct registered_server server;
	server.ip = SERVER_IP;
	server.port = SERVER_PORT;
	server.mem_size = MEMORY_SIZE;
	int size = sizeof(struct registered_server);
	write_data((char *) &server, 0, size);
}

void server_init() {
	register_cnt = 1;
	thread_pointer = 0;
	used_thread = (char*) malloc(THREAD_LIMIT);
	threads = (pthread_t*) malloc(sizeof(pthread_t) * THREAD_LIMIT);
	memory_control_init(MEMORY_SIZE);
}

void server_destroy() {
	for (int i = 0; i < THREAD_LIMIT; i++) {
		pthread_join(threads[i], NULL);
	}
	free(threads);
	free(used_thread);
	memory_control_destroy();
}

int main(int argc, char **argv) {
	evaluate_args(argc, argv);

	server_init();

	if (MAIN_SERVER) {
		printf("Self registered\n");
		self_register();
		struct registered_server* server = 
			(struct registered_server*) read_data(0, sizeof(struct registered_server));
		printf("%d %d %d\n", server->ip, server->port, server->mem_size);
		// TODO criar thread de log
	}

	printf("%d %d %d\n", SERVER_IP, SERVER_PORT, MEMORY_SIZE);
	
	while (1) {
	
	}

	return 0;
}

void evaluate_args(int argc, char **argv) {
	int server_ip_set = 0;
	int server_port_set = 0;
	int main_server_set = 0;
	int mem_size_set = 0;

	int pointer = 1;
	while (pointer < argc) {
		if (!strcmp(argv[pointer], "--server-ip")) {
			server_ip_set = 1;
			sscanf(" %d", argv[++pointer], &SERVER_IP);
		} else if (!strcmp(argv[pointer], "--server-port")) {
			server_port_set = 1;
			sscanf(" %d", argv[++pointer], &SERVER_PORT);
		} else if (!strcmp(argv[pointer], "--main-server")) {
			main_server_set = 1;
			MAIN_SERVER = 1;
		} else if (!strcmp(argv[pointer], "--mem-size")) {
			mem_size_set = 1;
			sscanf(" %d", argv[++pointer], &MEMORY_SIZE);
		}
		pointer++;
	}

	if (!server_ip_set) {
		SERVER_IP = LOCAL_HOST;
	}
	if (!server_port_set) {
		SERVER_PORT = DEFAULT_PORT;
	}
	if (!main_server_set) {
		MAIN_SERVER = 0;
	}
	if (!mem_size_set) {
		MEMORY_SIZE = DEFAULT_MEM_SIZE;
	}
}