#ifndef CORE_OPERATIONS_H_
#define CORE_OPERATIONS_H_

#include "types.h"
#include "generic_logger.h"

void process_select(select_input_t* input);
void process_insert(insert_input_t* input);
void process_create(create_input_t* input);
void process_describe(describe_input_t* input);
void process_drop(drop_input_t* input);
void process_journal();
void process_add(add_input_t* input);
void process_run(run_input_t* input);

#endif /* CORE_OPERATIONS_H_ */