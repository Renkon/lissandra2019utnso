#ifndef UTILS_NETWORK_H_
#define UTILS_NETWORK_H_

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include "network_types.h"
#include "serializer.h"

int __recv(int socket, void* buffer, int bytes_to_read);
int recv2(int socket, packet_t* packet);
int send2(int socket, packet_t* packet);
void build_packet(packet_t* packet, process_t process, socket_operation_t operation, bool keep_alive, int elements, int* elements_size, void* content, bool success);
void free_packet(packet_t* packet);

#endif /* UTILS_NETWORK_H_ */
