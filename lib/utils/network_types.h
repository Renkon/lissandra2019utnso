#ifndef NETWORK_TYPES_H_
#define NETWORK_TYPES_H_

#include "operation_types.h"
#include <stdbool.h>

// Tipos de operaciones posibles a transmitir por red
typedef enum {
	HANDSHAKE_IN,
	HANDSHAKE_OUT,
	SELECT_IN,
	SELECT_OUT,
	INSERT_IN,
	INSERT_OUT,
	CREATE_IN,
	CREATE_OUT,
	DESCRIBE_IN,
	DESCRIBE_OUT,
	DROP_IN,
	DROP_OUT,
	JOURNAL_IN,
	JOURNAL_OUT,
	GOSSIP_IN,
	GOSSIP_OUT,
	VALUE_IN,
	VALUE_OUT,
	MULTIINSERT_IN,
	MULTIINSERT_OUT,
	_LAST
} socket_operation_t;

typedef struct {
	process_t process;
	socket_operation_t operation;
	bool keep_alive;
	int elements;
	int* elements_size;
	bool success;
}__attribute__((packed)) header_t;

typedef struct {
	header_t header;
	void* content;
}__attribute__((packed)) packet_t;

char* get_process_name(process_t process);

#endif /* NETWORK_TYPES_H_ */
