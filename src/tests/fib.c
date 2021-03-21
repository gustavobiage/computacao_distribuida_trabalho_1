#include "../../templates/tests/fib.h"

int main(int argc, char ** argv) {
	if (argc < ARG_MIN) {
		printf("Quantidade de argumentos errado.\n");
		return -1;
	}

	struct server server;
	strcpy(server.ip, argv[SERVER_IP]);
	sscanf(argv[SERVER_PORT], " %d", &server.port);

	char v;
	v = 0;
	escreve(&server, 0, &v, 1);
	printf("fib[0] = %d\n", v);
	v = 1;
	escreve(&server, 1, &v, 1);
	printf("fib[1] = %d\n", v);

	int n;
	char v1, v2;
	char * buffer;
	sscanf(argv[PARAMETER], " %d ", &n);
	for (int i = 2; i <= n; i++) {
		buffer = le(&server, i-1, 1);
		v1 = buffer[0];
		free(buffer);
	
		buffer = le(&server, i-2, 1);
		v2 = buffer[0];
		free(buffer);

		v = (v1 + v2) % 128;
		escreve(&server, i, &v, 1);
		printf("fib[%d] %c 256 = %d\n", i, '%', v);
	}
	return 0;
}