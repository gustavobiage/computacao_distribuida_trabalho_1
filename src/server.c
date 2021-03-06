#include "../templates/server.h"

char SERVER_IP[15];
char* SERVER_NAME;
char* SLEEP_DURATION;
int SERVER_PORT;
int MAIN_SERVER;
int MEMORY_SIZE;
int BACKLOG;
int REGISTER_LIMIT;

struct linked_list server_list;
pthread_t * threads;

void self_register() {
	struct registered_server* server = (struct registered_server*) malloc(sizeof(struct registered_server));
	strcpy(server->ip, SERVER_IP);
	server->port = SERVER_PORT;
	server->mem_size = MEMORY_SIZE;
	push_front(&server_list, server);
}

void server_init() {
	memory_control_init(SERVER_NAME, MEMORY_SIZE);
	threads = (pthread_t*) malloc(sizeof(pthread_t) * BACKLOG);
}

void server_destroy() {
	for (int i = 0; i < BACKLOG; i++) {
		pthread_join(threads[i], NULL);
	}
	free(threads);
	memory_control_destroy();
}

void resolve_read(struct header header, int client_sockfd) {
	struct header response;
	int mem_size;
	int start = header.arg1;
	int length = header.arg2;
	struct iterator* it = iterator(&server_list);
	while (has_next(it)) {
		struct registered_server* server = (struct registered_server*) next(&it);
		mem_size = server->mem_size;
		if (start < mem_size && length > 0) {
			// Tem intersecção
			if (!strcmp(server->ip, SERVER_IP) && server->port == SERVER_PORT) {
				// Sou eu
				response.id = READ_DATA;
				response.arg1 = fmin(length, mem_size - start);
				// Dizer para o cliente esperar por dados
				write(client_sockfd, &response, sizeof(struct header));
				// Ler dados da memória
				char* buffer = read_data(start, response.arg1);
				// Enviar ao cliente dados lido da memória 
				write(client_sockfd, buffer, response.arg1);
				free(buffer);
				length = length - response.arg1;
			} else {
				// Redirecionar
				response.id = REDIRECT;
				write(client_sockfd, &response, sizeof(struct header));

				struct redirect redirect;
				redirect.server = *((struct server*) server);
				redirect.range.start = fmax(0, start);
				redirect.range.length = fmin(length, mem_size - start);
				
				write(client_sockfd, &redirect, sizeof(struct redirect));
				length = length - redirect.range.length;
			}
		}
		start = fmax(0, start - mem_size);
	}
	// Terminar conexão
	response.id = END_CONNECTION;
	write(client_sockfd, &response, sizeof(struct header));
}

void resolve_write(struct header header, int client_sockfd) {
	struct header response;
	int mem_size;
	int start = header.arg1;
	int length = header.arg2;
	struct iterator* it = iterator(&server_list);
	while (has_next(it)) {
		struct registered_server* server = (struct registered_server*) next(&it);
		mem_size = server->mem_size;
		if (start < mem_size && length > 0) {
			// Tem intersecção
			if (!strcmp(server->ip, SERVER_IP) && server->port == SERVER_PORT) {
				// Sou eu
				response.id = SEND_DATA;
				response.arg1 = fmin(length, mem_size - start);
				// Dizer para o cliente enviar dados a serem escritos
				write(client_sockfd, &response, sizeof(struct header));
				char buffer[response.arg1];
				// Receber do cliente dados a serem escritos
				read(client_sockfd, buffer, response.arg1);
				// Escrever na memória dados recebidos
				write_data(buffer, start, response.arg1);
				length = length - response.arg1;
			} else {
				// Redirecionar
				response.id = REDIRECT;
				write(client_sockfd, &response, sizeof(struct header));

				struct redirect redirect;
				redirect.server = *((struct server*) server);
				redirect.range.start = fmax(0, start);
				redirect.range.length = fmin(length, mem_size - start);
				write(client_sockfd, &redirect, sizeof(struct redirect));
				length = length - redirect.range.length;
			}
		}
		start = fmax(0, start - mem_size);
	}
	

	// Terminar conexão
	response.id = END_CONNECTION;
	write(client_sockfd, &response, sizeof(struct header));
}

void* resolve_request(void* arg) {
	struct thread_par* parameters = (struct thread_par*) arg;
	int client_sockfd = parameters->client_sockfd;
	char str[255];
	int received;

	struct header header;
	read(client_sockfd, &header, sizeof(struct header));
	if (header.id == READ_MEMORY) {
		resolve_read(header, client_sockfd);
	} else if (header.id == WRITE_MEMORY) {
		resolve_write(header, client_sockfd);
	}

	close(client_sockfd);
	pthread_t* self_pointer = parameters->self_pointer;
	struct c_queue* thread_queue = parameters->thread_queue;
	push(thread_queue, self_pointer); 
	free(parameters);
	pthread_exit(0);
}

void logger_init() {
	char command[1000], aux[1000];
	command[0] = '\0';
	strcat(command, "./logger");
	if (SLEEP_DURATION != NULL) {
		sprintf(aux, " --sleep-duration %s", SLEEP_DURATION);
		strcat(command, aux);
	}
	struct iterator* it = iterator(&server_list);
	while (has_next(it)) {
		struct registered_server* server = (struct registered_server*) next(&it);
		sprintf(aux, " --register-server %s::%d::%d", server->ip, server->port, server->mem_size);
		strcat(command, aux);
	}
	strcat(command, " &");
	system(command);
}

void prt_server_list() {
	struct iterator* it = iterator(&server_list);
	printf("Servidores registrados:\n");
	while (has_next(it)) {
		struct registered_server* server = (struct registered_server*) next(&it);
		printf("%s::%d::%d\n", server->ip, server->port, server->mem_size);
	}
}

int main(int argc, char **argv) {
	list_init(&server_list);
	evaluate_args(argc, argv);
	server_init();
	self_register();
	prt_server_list();

	pthread_t* thread;
	struct c_queue thread_queue;
	queue_init(&thread_queue, BACKLOG);
	for (int i = 0; i < BACKLOG; i++) {
		push(&thread_queue, &threads[i]);
	}

	int client_sockfd;

	// CREATE SOCKET
	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = SERVER_PORT;

	// Permite re-utilizar a mesma porta por vários processos.
	// Resolve o problema de testes consecutivos resultarem na porta ainda estar ocupada.
	int optval = 1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	// BIND AND LISTEN
	bind(server_sockfd, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
	listen(server_sockfd, 5);
	int client_len = sizeof(struct sockaddr_in);

	if (MAIN_SERVER) {
		logger_init();
	}

	printf("Server waiting ...\n");

	int i = 0;
	while (1) {
		thread = (pthread_t*) pop(&thread_queue);

		// ACCEPT (retorna client_sockfd)
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);	

		if (i++ >= BACKLOG) {
			pthread_join(*thread, NULL);
		}

		struct thread_par* parameters = (struct thread_par*) malloc(sizeof(struct thread_par)); 
		parameters->client_sockfd = client_sockfd;
		parameters->self_pointer = thread;
		parameters->thread_queue = &thread_queue;
		
		pthread_create(thread, NULL, resolve_request, parameters);
	}
	queue_destroy(&thread_queue);
	list_destroy(&server_list);
	return 0;
}

void evaluate_args(int argc, char **argv) {
	int server_name_set = 0;
	int server_ip_set = 0;
	int server_port_set = 0;
	int main_server_set = 0;
	int mem_size_set = 0;
	int backlog_set = 0;
	int sleep_duration_set = 0;

	int pointer = 1;
	while (pointer < argc) {
		if (!strcmp(argv[pointer], "--server-name")) {
			server_name_set = 1;
			char * server_name = argv[++pointer];
			int name_len = strlen(server_name) + 1;
			SERVER_NAME = (char*) malloc(sizeof(char) * name_len);
			strcpy(SERVER_NAME, server_name);
		} else if (!strcmp(argv[pointer], "--server-ip")) {
			server_ip_set = 1;
			sscanf(argv[++pointer], " %s", SERVER_IP);
		} else if (!strcmp(argv[pointer], "--server-port")) {
			server_port_set = 1;
			sscanf(argv[++pointer], " %d", &SERVER_PORT);
		} else if (!strcmp(argv[pointer], "--main-server")) {
			main_server_set = 1;
			MAIN_SERVER = 1;
		} else if (!strcmp(argv[pointer], "--mem-size")) {
			mem_size_set = 1;
			sscanf(argv[++pointer], " %d", &MEMORY_SIZE);
		} else if (!strcmp(argv[pointer], "--backlog")) {
			backlog_set = 1;
			sscanf(argv[++pointer], " %d", &BACKLOG);
		} else if (!strcmp(argv[pointer], "--register-server")) {
			char * server_data = argv[++pointer];
			int ip1, ip2, ip3, ip4;
			struct registered_server* server = 
				(struct registered_server*) malloc(sizeof(struct registered_server));
			sscanf(server_data, " %d.%d.%d.%d::%d::%d", &ip1, &ip2, &ip3, &ip4, &server->port, &server->mem_size);
			sprintf(server->ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			push_back(&server_list, (void *) server);
		} else if (!strcmp(argv[pointer], "--sleep-duration")) {
			sleep_duration_set = 1;
			int sleep_len = strlen(argv[++pointer]) + 1;
			SLEEP_DURATION = (char *) malloc(sizeof(char) * sleep_len);
			strcpy(SLEEP_DURATION, argv[pointer]);
		}
		pointer++;
	}

	if (!server_name_set) {
		int name_len = strlen(DEFAULT_SERVER_NAME) + 1;
		SERVER_NAME = (char*) malloc(sizeof(char) * name_len);
		strcpy(SERVER_NAME, DEFAULT_SERVER_NAME);
	}
	if (!server_ip_set) {
		strcpy(SERVER_IP, LOCAL_HOST);
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
	if (!backlog_set) {
		BACKLOG = DEFAULT_BACKLOG;
	}
	if (!sleep_duration_set) {
		SLEEP_DURATION = NULL;
	}
}