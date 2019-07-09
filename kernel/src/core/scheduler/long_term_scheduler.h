#ifndef CORE_SCHEDULER_LONG_TERM_SCHEDULER_H_
#define CORE_SCHEDULER_LONG_TERM_SCHEDULER_H_
#include "pcb.h"
#include "scheduler.h"

sem_t g_lts_semaphore;

void long_term_schedule();

#endif /* CORE_SCHEDULER_LONG_TERM_SCHEDULER_H_ */
