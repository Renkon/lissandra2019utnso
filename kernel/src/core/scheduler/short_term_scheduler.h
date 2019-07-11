#ifndef CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_
#define CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_

#include "scheduler.h"
#include "long_term_scheduler.h"
#include "pcb.h"
#include "utils/serializer.h"
#include "../statistics/stats.h"
#include "shared_types/shared_types.h"
#include <semaphore.h>
#include "statement.h"
#include "utils/operation_types.h"
#include "utils/dates.h"
#include "../describer/describer.h"

sem_t g_sts_semaphore;
sem_t g_inner_scheduler_semaphore;
t_list* __pcbs_to_ready;

void short_term_schedule();
void planifier_execute(void* arg);
void exec_next_statement(int processor);
void exec_remote(pcb_t* pcb, statement_t* statement);

void sem_post_neg(sem_t* semaphore);

void on_select(void* result, response_t* response);
void on_insert(void* result, response_t* response);
void on_create(void* result, response_t* response);
void on_describe(void* result, response_t* response);
void on_drop(void* result, response_t* response);
void on_journal(void* result, response_t* response);

void post_exec_statement(pcb_t* pcb, statement_t* current_statement);

#endif /* CORE_SCHEDULER_SHORT_TERM_SCHEDULER_H_ */
