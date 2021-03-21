#include "../templates/logger.h"

char * SLEEP_DURATION;

struct linked_list server_list;

void evaluate_args(int argc, char **argv);

void logger_init() {
	list_init(&server_list);
}

void logger_destroy() {
	free(SLEEP_DURATION);
	list_destroy(&server_list);
}

void * resolve_log(void * par) {
	char output[1000];
	struct timeval start;
	gettimeofday(&start, NULL);
	sprintf(output, "%s/%ld.log", OUTPUT_DIR, start.tv_usec);
	FILE* output_file = fopen(output, "w");

	struct iterator* it = iterator(&server_list);
	while (has_next(it)) {
		struct registered_server* server = (struct registered_server*) next(&it);
		int pointer = 0;
		// Como a memória pode ser na escala de GigaBytes
		// devemos ler a memória de pouco em pouco
		while (server->mem_size > 0) {
			int size = fmin(server->mem_size, 100);
			char* buffer = le((struct server*) server, pointer, size);
			fwrite(buffer, sizeof(char), size, output_file);
			pointer += size;
			server->mem_size -= size;
			// TODO free no buffer?
		}
	}
	fclose(output_file);
	pthread_exit(0);
}

int main(int argc, char** argv) {
	logger_init();
	evaluate_args(argc, argv);

	int amount;
	char unit;
	int time = 1000;
	sscanf(SLEEP_DURATION, "%d%c ", &amount, &unit);
	switch (unit) {
		case 'd': case 'D':
			time = time * 24;
		case 'h': case 'H':
			time = time * 60;
		case 'm': case 'M':
			time = time * 60;
		case 's': case 'S':
			break;
		default:
			time = 1000 * 60;
	}
	time = time * amount;

	pthread_t thread;

	int first = 1;
	while (1) {
		sleep(time);

		if (!first) {
			pthread_join(thread, NULL);
			first = 0;
		}

		pthread_create(&thread, NULL, resolve_log, NULL);
	}
	pthread_exit(0);
}

void evaluate_args(int argc, char **argv) {
	int set_output = 0;
	int set_sleep_duration = 0;

	int pointer = 1;
	while (pointer < argc) {
		if (!strcmp(argv[pointer], "--sleep-duration")) {
			set_sleep_duration = 1;
			int sleep_len = strlen(argv[++pointer]) + 1;
			SLEEP_DURATION = (char *) malloc(sizeof(char) * sleep_len);
			strcpy(SLEEP_DURATION, argv[pointer]);
		} else if (!strcmp(argv[pointer], "--register-server")) {
			char * server_data = argv[++pointer];
			int ip1, ip2, ip3, ip4;
			struct registered_server* server = 
				(struct registered_server*) malloc(sizeof(struct registered_server));
			sscanf(server_data, " %d.%d.%d.%d::%d::%d", &ip1, &ip2, &ip3, &ip4, &server->port, &server->mem_size);
			sprintf(server->ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			push_back(&server_list, (void *) server);
		}
		pointer++;
	}

	if (!set_sleep_duration) {
		int sleep_len = strlen(DEFAULT_SLEEP_DURATION) + 1;
		SLEEP_DURATION = (char *) malloc(sizeof(char) * sleep_len);
		strcpy(SLEEP_DURATION, DEFAULT_SLEEP_DURATION);
	}
}
