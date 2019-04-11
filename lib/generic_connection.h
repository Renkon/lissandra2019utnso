#ifndef GENERIC_CONNECTION_H_
#define GENERIC_CONNECTION_H_
#define LISTEN_QUEUE 5
#define BUFFER_SIZE 1024

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include "generic_logger.h"

typedef struct {
	int socket;
	char* buffer;
} conn_thread_args_t;

bool init_server(int listen_port);
bool setup_server(int listen_port);

#endif /* GENERIC_CONNECTION_H_ */
