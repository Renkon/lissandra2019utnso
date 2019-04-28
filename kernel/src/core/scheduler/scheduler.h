#ifndef CORE_SCHEDULER_SCHEDULER_H_
#define CORE_SCHEDULER_SCHEDULER_H_

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "../../config.h"
#include "generic_logger.h"
#include "commons/collections/list.h"
#include "utils/operation_types.h"
#include "../utils/pid_utils.h"
#include "short_term_scheduler.h"
#include "long_term_scheduler.h"

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
} pcb_t;

typedef struct {
	t_list* new;
	t_list* ready;
	t_list* exit;
	t_list* exec_semaphores;
	t_list* exec;
} scheduler_queues_t;

scheduler_queues_t g_scheduler_queues;

void init_scheduler();
void setup_scheduler_queues();
pcb_t* get_new_pcb();
void delete_pcb(pcb_t* pcb);

#endif /* CORE_SCHEDULER_SCHEDULER_H_ */
