#ifndef GENERIC_CONNECTION_H_
#define GENERIC_CONNECTION_H_
#define LISTEN_QUEUE 5

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include "generic_logger.h"
#include "utils/network.h"
#include "operation_types.h"
#include "network_types.h"

typedef struct {
	int port;
	process_t process;
} conn_thread_args_t;

typedef struct {
	int socket;
	process_t process;
} conn_args_t;

bool init_server(int listen_port, process_t process);
bool setup_server(void* args);
void handle_request(void* args);
bool valid_source(process_t me, process_t client);

#endif /* GENERIC_CONNECTION_H_ */
