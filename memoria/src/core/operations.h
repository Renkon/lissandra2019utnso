#ifndef CORE_OPERATIONS3_H_
#define CORE_OPERATIONS3_H_

#include "../memory_tables.h"
#include "utils/operation_types.h"
#include "generic_client.h"
#include "../config.h"
#include "generic_logger.h"
#include <stdbool.h>
#include "../memory_utils.h"



void process_select(select_input_t* input);
void process_insert(insert_input_t* input);
void process_create(create_input_t* input);
void process_describe(describe_input_t* input);
void process_drop(drop_input_t* input);
void process_journal();


void select_callback(void* response);
void create_callback(void* response);
void describe_callback(void* response);
void select_callback(void* response);



#endif /* CORE_OPERATIONS3_H_ */
