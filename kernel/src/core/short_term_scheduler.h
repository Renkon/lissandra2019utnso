#ifndef CORE_SHORT_TERM_SCHEDULER_H_
#define CORE_SHORT_TERM_SCHEDULER_H_

#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "../config.h"
#include "generic_logger.h"
#include "commons/collections/list.h"
#include "utils/operation_types.h"
#include "utils/pid_utils.h"

typedef enum {
	NEW,
	READY,
	EXEC,
	EXIT
} t_state;

typedef struct {
	int process_id;
	t_state state;
	int program_counter;
	t_list* statements;
	bool errors;
} pcb_t;

typedef struct {
	t_list* new;
	t_list* ready;
	t_list* exit;
	t_list* exec;
} scheduler_queues_t;

typedef struct {
	operation_t operation;
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;
	add_input_t* add_input;
} statement_t;

scheduler_queues_t g_scheduler_queues;

void init_scheduler();
void scheduler();
void schedule();
pcb_t* get_new_pcb();
void delete_pcb(pcb_t* pcb);

#endif /* CORE_SHORT_TERM_SCHEDULER_H_ */
