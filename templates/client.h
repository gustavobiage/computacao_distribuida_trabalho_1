#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "client_server.h"
#include "vector.h"

int open_connection(struct server* server);

void close_connection(int sockfd);

void escreve(struct server* server, int posicao, char * buffer, int tam_buffer);

char* le(struct server* server, int posicao, int tamanho);