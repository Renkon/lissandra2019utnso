#include "scheduler.h"

void init_scheduler() {
	pthread_t planifier_thread;

	setup_pid_generator();
	setup_scheduler_queues();

	if (pthread_create(&planifier_thread, NULL, (void*) short_term_schedule, NULL)) {
		log_e("No se pudo inicializar el hilo de planificacion a corto plazo");
	}

	if (pthread_create(&planifier_thread, NULL, (void*) long_term_schedule, NULL)) {
		log_e("No se pudo inicializar el hilo de planificacion a largo plazo");
	}
}

void setup_scheduler_queues() {
	pthread_t exec_thread;
	int* exec_id;
	sem_t* semaphore_init;
	sem_t* semaphore_end;

	g_scheduler_queues.new = list_create();
	g_scheduler_queues.ready = list_create();
	g_scheduler_queues.exec = list_create();
	g_scheduler_queues.exec_semaphores_init = list_create();
	g_scheduler_queues.exec_semaphores_end = list_create();
	g_scheduler_queues.exit = list_create();

	for (int i = 0; i < g_config.multiprocessing; i++) {
		exec_id = malloc(sizeof(int));
		*exec_id = i;
		semaphore_init = malloc(sizeof(sem_t));
		semaphore_end = malloc(sizeof(sem_t));
		sem_init(semaphore_init, 0, 0);
		sem_init(semaphore_end, 0, 0);

		list_add(g_scheduler_queues.exec, NULL);
		list_add(g_scheduler_queues.exec_semaphores_init, semaphore_init);
		list_add(g_scheduler_queues.exec_semaphores_end, semaphore_end);

		if (pthread_create(&exec_thread, NULL, (void*) planifier_execute, (void*) exec_id)) {
			log_e("No se pudo inicializar el hilo %i de Exec", i);
			return;
		}
	}

	log_i("Se inicializaron las colas del planificador");
}

pcb_t* get_new_pcb() {
	pcb_t* pcb = malloc(sizeof(pcb_t));

	pcb->process_id = generate_pid();
	pcb->state = NEW;
	pcb->program_counter = 0;
	pcb->errors = false;
	pcb->statements = list_create();
	pcb->__recently_ready = false;

	return pcb;
}

void delete_pcb(pcb_t* pcb) {
	remove_pid(pcb->process_id);

	for (int i = 0; i < list_size(pcb->statements); i++) {
		statement_t* statement = (statement_t*) list_get(pcb->statements, i);
		switch (statement->operation) {
			case SELECT:
				free(statement->select_input->table_name);
				free(statement->select_input);
			break;
			case CREATE:
				free(statement->create_input->table_name);
				free(statement->create_input);
			break;
			case INSERT:
				free(statement->insert_input->table_name);
				free(statement->insert_input->value);
				free(statement->insert_input);
			break;
			case DROP:
				free(statement->drop_input->table_name);
				free(statement->drop_input);
			break;
			case DESCRIBE:
				free(statement->describe_input->table_name);
				free(statement->describe_input);
			break;
			default:
			break;
		}
		free(statement);
	}

	list_destroy(pcb->statements);
	free(pcb);
}
