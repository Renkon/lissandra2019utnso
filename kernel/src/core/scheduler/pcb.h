#include <stdbool.h>
#include "scheduler.h"
#include "../statistics/stats.h"

#ifndef CORE_SCHEDULER_PCB_H_
#define CORE_SCHEDULER_PCB_H_

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
	int quantum;
	t_list* statements;
	bool errors;
	bool __recently_ready;
	bool __recently_exec;
	int processor;
	stats_t* last_execution_stats;
} pcb_t;

#endif /* CORE_SCHEDULER_PCB_H_ */
