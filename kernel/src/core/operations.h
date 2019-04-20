#ifndef CORE_OPERATIONS_H_
#define CORE_OPERATIONS_H_

#include "utils/operation_types.h"
#include "generic_client.h"
#include "../config.h"
#include "generic_logger.h"
#include "utils/file.h"
#include "scheduler/scheduler.h"

void process_select(select_input_t* input);
void process_insert(insert_input_t* input);
void process_create(create_input_t* input);
void process_describe(describe_input_t* input);
void process_drop(drop_input_t* input);
void process_journal();
void process_add(add_input_t* input);
void process_run(run_input_t* input);
void process_metrics();
bool on_inner_run_request(t_list* statements, run_input_t* input, bool free_input);
void add_statement(t_list* statements, operation_t operation, char* command);

#endif /* CORE_OPERATIONS_H_ */
