#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "operation_types.h"
#include "network_types.h"
#include "../shared_types/shared_types.h"
#include "commons/collections/list.h"

#ifndef UTILS_SERIALIZER_H_
#define UTILS_SERIALIZER_H_

typedef struct {
	int elements;
	int* elements_size;
} elements_network_t;

elements_network_t elements_select_in_info(void* input);
elements_network_t elements_select_out_info(void* input);
elements_network_t elements_insert_in_info(void* input);
elements_network_t elements_insert_out_info(void* input);
elements_network_t elements_create_in_info(void* input);
elements_network_t elements_create_out_info(void* input);
elements_network_t elements_describe_in_info(void* input);
elements_network_t elements_describe_out_info(void* input);
elements_network_t elements_drop_in_info(void* input);
elements_network_t elements_drop_out_info(void* input);
elements_network_t elements_journal_in_info(void* input);
elements_network_t elements_journal_out_info(void* input);
elements_network_t init_elements_info(int elements);
void serialize_content(void* to, socket_operation_t operation, void* from);
void* deserialize_content(void* from, socket_operation_t operation, int elements, int* elements_size);
void free_deserialized_content(void* content, socket_operation_t operation);

#endif /* UTILS_SERIALIZER_H_ */
