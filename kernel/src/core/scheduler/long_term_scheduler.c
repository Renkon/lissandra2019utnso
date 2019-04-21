#include "long_term_scheduler.h"

void long_term_schedule() {
	pthread_detach(pthread_self());

	// Este bloque se encarga de liberar los procesos en exit, o de mandar a ready los scripts ya inicializados
	while (true) {
		usleep(g_config.lts_delay * 1000);

		if (list_size(g_scheduler_queues.new) > 0) {
			pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.new, 0);
			pcb->state = READY;

			// Lo borro de NEW y lo muevo a READY
			list_remove(g_scheduler_queues.new, 0);
			list_add(g_scheduler_queues.ready, pcb);

			// Skippeamos lo q queda de la iteracion
			// las de NEW son mas prioritarias que las de EXIT
			// puede haber starvation si hay continuamente procesos entrando en new
			continue;
		}

		if (list_size(g_scheduler_queues.exit) > 0) {
			pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.exit, 0);

			list_remove(g_scheduler_queues.exit, 0);
			// TODO: logica para borrar el PCB y toda esa wea
		}
	}
}
