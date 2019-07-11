#include "long_term_scheduler.h"

void long_term_schedule() {
	pthread_detach(pthread_self());

	sem_init(&g_lts_semaphore, 0, 0);

	// Este bloque se encarga de liberar los procesos en exit, o de mandar a ready los scripts ya inicializados
	// Es el LTS, O sea que esta siempre en ejecucion (en idle en el peor de los casos)
	while (true) {
		sem_wait(&g_lts_semaphore);
		usleep(g_config.lts_delay * 1000);

		if (list_size(g_scheduler_queues.new) > 0) {
			pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.new, 0);
			pcb->state = READY;

			// Lo borro de NEW y lo muevo a READY
			list_remove(g_scheduler_queues.new, 0);
			list_add(g_scheduler_queues.ready, pcb);

			log_d("LTS nuevo proceso PID %i", pcb->process_id);

			sem_post_neg(&g_sts_semaphore);

			// Skippeamos lo q queda de la iteracion
			// las de NEW son mas prioritarias que las de EXIT
			// puede haber starvation si hay continuamente procesos entrando en new
			continue;
		}

		if (list_size(g_scheduler_queues.exit) > 0) {
			sem_wait(&g_inner_scheduler_semaphore);
			pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.exit, 0);

			log_d("LTS fin de proceso PID %i", pcb->process_id);
			list_remove(g_scheduler_queues.exit, 0);

			for (int i = 0; i < list_size(__pcbs_to_ready); i++) {
				if (list_get(__pcbs_to_ready, i) == pcb) {
					list_replace(__pcbs_to_ready, i, NULL);
					break;
				}
			}
			delete_pcb(pcb);
			sem_post(&g_inner_scheduler_semaphore);
		}
	}
}
