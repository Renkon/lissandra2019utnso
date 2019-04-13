#ifndef NETWORK_TYPES_H_
#define NETWORK_TYPES_H_

#include "operation_types.h"
#include <stdbool.h>

// Tipos de operaciones posibles a transmitir por red
typedef enum {
	HANDSHAKE_NET,
	SELECT_NET,
	INSERT_NET,
	CREATE_NET,
	DESCRIBE_NET,
	DROP_NET,
	JOURNAL_NET,
	ADD_NET,
	RUN_NET,
	GOSSIP_NET
} socket_operation_t;

typedef struct {
	process_t process;
	socket_operation_t operation;
	bool keep_alive;
	int content_length;
}__attribute__((packed)) header_t;

typedef struct {
	header_t header;
	void* content;
}__attribute__((packed)) packet_t;

char* get_process_name(process_t process);

#endif /* NETWORK_TYPES_H_ */
