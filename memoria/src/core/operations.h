#ifndef CORE_OPERATIONS3_H_
#define CORE_OPERATIONS3_H_

#include "utils/operation_types.h"
#include "generic_client.h"
#include "../config.h"
#include "generic_logger.h"

void process_select(select_input_t* input);
void process_insert(insert_input_t* input);
void process_create(create_input_t* input);
void process_describe(describe_input_t* input);
void process_drop(drop_input_t* input);
void process_journal();
void select_callback(void* response);

#endif /* CORE_OPERATIONS3_H_ */
