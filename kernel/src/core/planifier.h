#ifndef CORE_PLANIFIER_H_
#define CORE_PLANIFIER_H_

#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "../config.h"
#include "generic_logger.h"
#include "commons/collections/list.h"

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
} pcb_t;

typedef struct {
	t_list* new;
	t_list* ready;
	t_list* exit;
	t_list* exec;
} planifier_queues_t;

planifier_queues_t g_planifier_queues;

void init_planifier();
void setup_planifier_queues();
void planify();

#endif /* CORE_PLANIFIER_H_ */
