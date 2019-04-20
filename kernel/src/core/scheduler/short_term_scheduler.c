#include "short_term_scheduler.h"

void short_term_schedule() {
	pthread_detach(pthread_self());

	setup_scheduler_queues();

	// Este bloque se ejecuta continuamente, y es la mente maestra del planificador
	while (true) {
		for (int i = 0; i < list_size(g_scheduler_queues.new); i++) {
			pcb_t* pcb = list_get(g_scheduler_queues.new, i);
			log_i("Proceso %i, lineas a ejecutar %i", pcb->process_id, list_size(pcb->statements));
			for (int j = 0; j < list_size(pcb->statements); j++) {
				log_i("%i -> %i", j + 1, ((statement_t*) list_get(pcb->statements, j))->operation);
			}
		}
		usleep(g_config.execution_delay * 1000);
	}
}
