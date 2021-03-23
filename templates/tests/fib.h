#include <stdlib.h>

#include "../client.h"

#define SERVER_IP 1
#define SERVER_PORT 2
#define PARAMETER 3
#define ARG_MIN 4

#define INTEGER sizeof(int)
#define MAX_INTEGER 2147483647

#define MOD(x) ((x)%MAX_INTEGER)

#define POS(n) ((int)((n)*INTEGER))