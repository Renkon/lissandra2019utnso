#ifndef GENERIC_CLIENT_H_
#define GENERIC_CLIENT_H_

#include <arpa/inet.h>
#include "generic_server.h"

typedef struct {
	process_t process;
	char* ip;
	int port;
	socket_operation_t operation;
	void* content;
	int content_length;
	void (*callback)(void*);
} client_conn_args_t;

int setup_connection(process_t process, char* ip, int port);
void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int content_length, void (*callback)(void*));
void do_request(void* args);

#endif /* GENERIC_CLIENT_H_ */
