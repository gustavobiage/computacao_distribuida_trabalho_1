#include "../../templates/tests/fib.h"

int main(int argc, char ** argv) {
	if (argc < ARG_MIN) {
		printf("Quantidade de argumentos errado.\n");
		return -1;
	}

	struct server server;
	strcpy(server.ip, argv[SERVER_IP]);
	sscanf(argv[SERVER_PORT], " %d", &server.port);

	int v;
	v = 0;
	escreve(&server, POS(0), (char*) &v, INTEGER);
	printf("fib[0] = %d\n", v);
	v = 1;
	escreve(&server, POS(1), (char*) &v, INTEGER);
	printf("fib[1] = %d\n", v);

	int n;
	int v1, v2;
	char * buffer;
	sscanf(argv[PARAMETER], " %d ", &n);
	for (int i = 2; i <= n; i++) {
		buffer = le(&server, POS(i-1), INTEGER);
		v1 = *((int*) buffer);
		free(buffer);
	
		buffer = le(&server, POS(i-2), INTEGER);
		v2 = *((int*) buffer);
		free(buffer);

		v = MOD((long int) v1 + v2);
		escreve(&server, POS(i), (char*) &v, INTEGER);
		printf("fib[%d] %c %d = %d\n", i, '%', MAX_INTEGER, v);
	}
	return 0;
}