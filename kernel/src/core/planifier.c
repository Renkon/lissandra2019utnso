#include "planifier.h"

void init_planifier() {
	pthread_t planifier_thread;

	if (pthread_create(&planifier_thread, NULL, (void*) planify, NULL)) {
		log_e("No se pudo inicializar el hilo de planificacion");
	}
}

void setup_planifier_queues() {
	g_planifier_queues.new = list_create();
	g_planifier_queues.ready = list_create();
	g_planifier_queues.exec = list_create();
	g_planifier_queues.exit = list_create();
	log_i("Se inicializaron las colas del planificador");
}

void planify() {
	pthread_detach(pthread_self());

	setup_planifier_queues();

	// Este bloque se ejecuta continuamente, y es la mente maestra del planificador
	while (true) {
		log_i("Durmiendo por %i ms", g_config.execution_delay);
		usleep(g_config.execution_delay * 1000);
	}
}
