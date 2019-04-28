#include "short_term_scheduler.h"

void short_term_schedule() {
	pcb_t* pcb;
	sem_t* semaphore;
	t_list* __pcbs_to_ready = list_create();

	pthread_detach(pthread_self());

	// Este bloque se ejecuta continuamente, y es la mente maestra del planificador
	while (true) {
		usleep(g_config.execution_delay * 1000);

		for (int i = 0; i < list_size(g_scheduler_queues.exec); i++) {
			if ((pcb = (pcb_t*) list_get(g_scheduler_queues.exec, i)) == NULL) {
				// No hay proceso en este slot de lo que esta en ejecucion
				// Procedemos a obtener el primer elemento y lo dejamos en ready
				if (!list_is_empty(g_scheduler_queues.ready) && !(pcb = (pcb_t*) list_get(g_scheduler_queues.ready, 0))->__recently_ready) {
					// La lista de ready no esta vacia, asi que movemos lo primero de ready a exec
					list_remove(g_scheduler_queues.ready, 0);
					list_remove(g_scheduler_queues.exec, i);
					list_add_in_index(g_scheduler_queues.exec, i, pcb);
					pcb->state = EXEC;
					pcb->quantum = 0;

					log_t("Proceso %i pasado a Exec", pcb->process_id);
				}
			} else {
				// Revisamos si termino de ejecutar
				if (pcb->program_counter >= list_size(pcb->statements)) {
					list_remove(g_scheduler_queues.exec, i);
					list_add(g_scheduler_queues.exit, pcb);
					list_add_in_index(g_scheduler_queues.exec, i, NULL);
					pcb->state = EXIT;

					log_t("Proceso %i finalizo su ejecucion", pcb->process_id);
				} else if (pcb->quantum >= g_config.quantum) {
					// Termino su ciclo de quantum
					list_remove(g_scheduler_queues.exec, i);
					list_add(g_scheduler_queues.ready, pcb);
					list_add(__pcbs_to_ready, pcb);
					pcb->state = READY;
					pcb->__recently_ready = true;
					list_add_in_index(g_scheduler_queues.exec, i, NULL);

					log_t("Proceso %i vuelve a cola de listos", pcb->process_id);
				} else {
					semaphore = (sem_t*) list_get(g_scheduler_queues.exec_semaphores, i);
					sem_post(semaphore);
					log_t("Ejecutando en otro thread %i", i);
					// Aca el planificador ejecuta
					pcb->quantum++;
					sem_wait(semaphore);
				}
			}
		}

		for (int i = 0; i < list_size(__pcbs_to_ready); i++)
			((pcb_t*) list_get(__pcbs_to_ready, i))->__recently_ready = false;
		list_clean(__pcbs_to_ready);
	}
}

void planifier_execute(void* arg) {
	int exec_id = *((int*) arg);
	sem_t* semaphore = (sem_t*) list_get(g_scheduler_queues.exec_semaphores, exec_id);
	pcb_t* pcb;

	while (true) {
		sem_wait(semaphore);
		log_t("Ejecuto en thread %i", exec_id);
		pcb = (pcb_t*) list_get(g_scheduler_queues.exec, exec_id);
		exec_statement((statement_t*) list_get(pcb->statements, pcb->program_counter++));
		sem_post(semaphore);
	}
}

void exec_statement(statement_t* statement) {
	select_input_t* input = statement->select_input;
	log_t(input->table_name);
}
