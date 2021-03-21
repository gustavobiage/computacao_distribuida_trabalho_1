#include "../templates/client.h"

int open_connection(struct server* server) {
	int sockfd;
	int len, result;
	struct sockaddr_in address;
	struct in_addr addr;
	inet_aton(server->ip, &addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_port = server->port;
	address.sin_addr = addr;

	// Permite re-utilizar a mesma porta por vários processos.
	// Resolve o problema de testes consecutivos resultarem na porta ainda estar ocupada.
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *) &address, len);
	if (result == -1) {
		return -1;
	}
	return sockfd;
}

void close_connection(int sockfd) {
	close(sockfd);
}

void escreve(struct server* server, int posicao, char * buffer, int tam_buffer) {
	int sockfd = open_connection(server);
	int len = sizeof(struct header);
	int pointer = 0;
	struct header header;
	struct header response;
	header.id = WRITE_MEMORY;
	header.arg1 = posicao;
	header.arg2 = tam_buffer;
	write(sockfd, &header, len);
	struct vector redirects;
	vector_init(&redirects);

	while (1) {
		read(sockfd, &response, len);
		if (response.id == SEND_DATA) {
			int data_length = response.arg1;
			write(sockfd, buffer, pointer + data_length);
			pointer += data_length;
		} else if (response.id == REDIRECT) {
			struct redirect* redirect = (struct redirect*) malloc(sizeof(struct redirect));
			read(sockfd, redirect, sizeof(struct redirect));
			vector_push_back(&redirects, redirect);
		} else if (response.id == END_CONNECTION) {
			break;
		}
	}
	close_connection(sockfd);	

	for (int i = 0; i < redirects.size; i++) {
		struct redirect* redirect = vector_get(&redirects, i);
		int start = redirect->range.start;
		int length = redirect->range.length;
		escreve(&redirect->server, start, buffer + pointer, length);
		pointer += length;
		free(redirect);
	}
	vector_destroy(&redirects);
	return buffer;
}

char* le(struct server* server, int posicao, int tamanho) {
	int sockfd = open_connection(server);
	int len = sizeof(struct header);
	int pointer = 0;
	struct header header;
	struct header response;
	header.id = READ_MEMORY;
	header.arg1 = posicao;
	header.arg2 = tamanho;
	write(sockfd, &header, len);
	struct vector redirects;
	vector_init(&redirects);
	char * buffer = (char*) malloc(sizeof(char)*tamanho);
	while (1) {
		read(sockfd, &response, len);
		if (response.id == READ_DATA) {
			int data_length = response.arg1;
			read(sockfd, buffer + pointer, data_length*sizeof(char));
			pointer += data_length;
		} else if (response.id == REDIRECT) {
			struct redirect* redirect = (struct redirect*) malloc(sizeof(struct redirect));
			read(sockfd, redirect, sizeof(struct redirect));
			vector_push_back(&redirects, redirect);
		} else if (response.id == END_CONNECTION) {
			break;
		}
	}
	close_connection(sockfd);

	for (int i = 0; i < redirects.size; i++) {
		struct redirect* redirect = vector_get(&redirects, i);
		int start = redirect->range.start;
		int length = redirect->range.length;
		char * rbuffer = le(&redirect->server, start, length);
		memcpy(buffer + pointer, rbuffer, sizeof(char)*length);
		pointer += length;
		free(rbuffer);
		free(redirect);
	}
	vector_destroy(&redirects);
	return buffer;
}