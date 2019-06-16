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
	int elements;
	int* elements_length;
	void (*callback)(void*);
	bool success;
} client_conn_args_t;

int setup_connection(process_t process, char* ip, int port);
void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int elements, int* elements_length, void (*callback)(void*), bool success);
void do_request(void* args);

#endif /* GENERIC_CLIENT_H_ */
