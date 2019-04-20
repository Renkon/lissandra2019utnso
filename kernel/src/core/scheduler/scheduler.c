#include "scheduler.h"

void init_scheduler() {
	pthread_t planifier_thread;

	setup_pid_generator();

	if (pthread_create(&planifier_thread, NULL, (void*) short_term_schedule, NULL)) {
		log_e("No se pudo inicializar el hilo de planificacion a corto plazo");
	}

	if (pthread_create(&planifier_thread, NULL, (void*) short_term_schedule, NULL)) {
		log_e("No se pudo inicializar el hilo de planificacion a largo plazo");
	}
}

void setup_scheduler_queues() {
	g_scheduler_queues.new = list_create();
	g_scheduler_queues.ready = list_create();
	g_scheduler_queues.exec = list_create();
	g_scheduler_queues.exit = list_create();
	log_i("Se inicializaron las colas del planificador");
}

pcb_t* get_new_pcb() {
	pcb_t* pcb = malloc(sizeof(pcb_t));

	pcb->process_id = generate_pid();
	pcb->state = NEW;
	pcb->program_counter = 0;
	pcb->errors = false;
	pcb->statements = list_create();

	return pcb;
}

void delete_pcb(pcb_t* pcb) {
	remove_pid(pcb->process_id);
	list_destroy(pcb->statements);
	free(pcb);
}

