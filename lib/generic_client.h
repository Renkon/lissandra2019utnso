#ifndef GENERIC_CLIENT_H_
#define GENERIC_CLIENT_H_

#include <arpa/inet.h>
#include "generic_server.h"

int setup_connection(process_t process, char* ip, int port, header_t* header_send, header_t* header_recv, packet_t* packet);
void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int content_length, void (*callback)(void*));
void release_request_resources(header_t* header1, header_t* header2, packet_t* packet);

#endif /* GENERIC_CLIENT_H_ */
