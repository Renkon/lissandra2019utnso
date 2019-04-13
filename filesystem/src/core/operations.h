#ifndef CORE_OPERATIONS2_H_
#define CORE_OPERATIONS2_H_

#include <operation_types.h>
#include "generic_logger.h"

void process_select(select_input_t* input);
void process_insert(insert_input_t* input);
void process_create(create_input_t* input);
void process_describe(describe_input_t* input);
void process_drop(drop_input_t* input);

#endif /* CORE_OPERATIONS2_H_ */
