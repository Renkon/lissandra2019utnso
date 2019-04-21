#include "short_term_scheduler.h"

void short_term_schedule() {
	pthread_detach(pthread_self());

	// Este bloque se ejecuta continuamente, y es la mente maestra del planificador
	while (true) {
		log_i("new: %i ready: %i", list_size(g_scheduler_queues.new), list_size(g_scheduler_queues.ready));
		usleep(g_config.execution_delay * 1000);
	}
}
