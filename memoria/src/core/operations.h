#ifndef CORE_OPERATIONS3_H_
#define CORE_OPERATIONS3_H_

#include "../memory_tables.h"
#include "utils/operation_types.h"
#include "generic_client.h"
#include "../config.h"
#include "generic_logger.h"
#include <stdbool.h>
#include "../memory_utils.h"
#include "utils/response.h"
#include "utils/serializer.h"

void process_select(select_input_t* input, response_t* response);
void process_insert(insert_input_t* input, response_t* response);
void process_create(create_input_t* input, response_t* response);
void process_describe(describe_input_t* input, response_t* response);
void process_drop(drop_input_t* input, response_t* response);
void process_journal(response_t* response);


void select_callback(void* result, response_t* response);
void create_callback(void* result, response_t* response);
void describe_callback(void* result, response_t* response);

void cleanup_select_input(void* input);

/* Ir a buscar el value */
void get_value_from_filesystem();
void get_value_callback(void* result, response_t* response);

#endif /* CORE_OPERATIONS3_H_ */
