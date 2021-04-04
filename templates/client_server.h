#ifndef _CLIENT_SERVER
#define _CLIENT_SERVER

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
	#define READ_MEMORY 0
	#define WRITE_MEMORY 2
	#define REGISTER_SERVER 3

	// response
	#define REDIRECT 4
	#define SEND_DATA 10
	#define READ_DATA 11
	#define DATA 5
	#define END_CONNECTION 6
	#define ERROR 7

	struct header {
		int id;
		int arg1, arg2;
	};

	struct server {
		// ipv4 é a representação de 4 bytes separados por pontos.
		// Cada byte pode ir de [0, 255], contendo no máximo 3 caracteres,
		// No final, temos algo no formato "%d%d%d.%d%d%d.%d%d%d.%d%d%d\0"
		char ip[16];
		int port;
	};

	struct registered_server {
		char ip[16];
		int port;
		int mem_size;
	};

	struct redirect {
		struct mem_range range;
		struct server server;
	};

#endif
