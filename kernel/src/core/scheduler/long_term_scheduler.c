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

			log_t("LTS nuevo proceso PID %i", pcb->process_id);

			// Skippeamos lo q queda de la iteracion
			// las de NEW son mas prioritarias que las de EXIT
			// puede haber starvation si hay continuamente procesos entrando en new
			continue;
		}

		if (list_size(g_scheduler_queues.exit) > 0) {
			pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.exit, 0);

			log_t("LTS fin de proceso PID %i", pcb->process_id);
			list_remove(g_scheduler_queues.exit, 0);
			delete_pcb(pcb);
		}
	}
}
