#include "../template/server.h"

int SERVER_IP;
int SERVER_PORT;
int MAIN_SERVER;
int MEMORY_SIZE;
int THREAD_LIMIT;
int REGISTER_LIMIT;

char * used_thread;
int register_cnt;
int thread_pointer;

pthread_mutex_t mutex;
|----|---------------|
|----| linked list
|------file-------|

// I) Transformar para memória em arquivo
// II) Realizar implementação de socket no servidor
// III) Implementar linked list para armazenamento de informações sobre outros servidores
// IV) Implementar redirecionamento

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

void* resolve_request(void* arg) {
	printf("THREAD START\n");
	struct thread_par* parameters = (struct thread_par*) arg;
	int client_sockfd = parameters->client_sockfd;
	
	// TODO resolve request

	close(client_sockfd);
	pthread_t* self_pointer = parameters->self_pointer;
	struct c_queue* thread_queue = parameters->thread_queue;
	push(thread_queue, self_pointer); 
	free(parameters);
	printf("THREAD END\n");
	pthread_exit(0);
}

int main(int argc, char **argv) {
	evaluate_args(argc, argv);

	server_init();

	if (MAIN_SERVER) {
		printf("Self registered\n");
		self_register();
	}

	printf("%d %d %d\n", SERVER_IP, SERVER_PORT, MEMORY_SIZE);

	pthread_t threads[THREAD_LIMIT];
	pthread_t* thread;
	struct c_queue thread_queue;
	queue_init(&thread_queue, THREAD_LIMIT);
	for (int i = 0; i < THREAD_LIMIT; i++) {
		push(&thread_queue, &threads[i]);
	}

	int i = 0;

	printf("Server waiting ...");

	// CREATE SOCKET
	// BIND AND LISTEN

	while (1) {
		thread = (pthread_t*) pop(&thread_queue);
		// ACCEPT (retorna client_sockfd)
		if (i++ >= THRED_LIMIT) {
			pthread_join(*thread, NULL)
		}

		struct thread_par* parameters = (struct thread_par*) malloc(sizeof(struct thread_par)); 
		parameters->client_sockfd = client_sockfd;
		parameters->self_pointer = thread;
		parameters->thread_queue = &thread_queue;
		
		pthread_init(thread, NULL, resolve_request, parameters);
	}
	queue_destroy(&thread_queue);
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