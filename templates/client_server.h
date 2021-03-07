#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "memory_utils.h"

// request
#define READ_DATA 0
#define WRITE_DATA 2
#define REGISTER_SERVER 3

// response
#define REDIRECT 4
#define DATA 5
#define END_CONNECTION 6
#define ERROR 7

struct client_server_header {
	int id;
	int value;
};

struct registered_server {
	// ipv4 é a representação de 4 bytes separados por pontos
	// Cada byte pode ir de [0, 255], contendo no máximo 3 caracteres.
	char ip[15]; 
	int port;
	int mem_size;
};

struct redirect_response {
	struct mem_range range;
	struct registered_server server;
};