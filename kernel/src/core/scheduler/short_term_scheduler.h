#ifndef CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_
#define CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_

#include "scheduler.h"
#include "pcb.h"
#include "../statistics/stats.h"

typedef struct {
	operation_t operation;
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;
} statement_t;

void short_term_schedule();
void planifier_execute(void* arg);
void exec_next_statement(int processor);
void on_statement_failure(pcb_t* pcb);

#endif /* CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_ */
