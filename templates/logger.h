#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#include "client.h"
#include "server.h"

#define DEFAULT_SLEEP_DURATION "5m"
#define OUTPUT_DIR "resources/tmp"

void logger_init();

void logger_destroy();

void* resolve_log(void*);