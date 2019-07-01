#include "short_term_scheduler.h"

void short_term_schedule() {
	pcb_t* pcb;
	sem_t* semaphore_init;

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
					semaphore_init = (sem_t*) list_get(g_scheduler_queues.exec_semaphores_init, i);
					pcb->processor = i;
					sem_post(semaphore_init);
					// Aca el planificador ejecuta
					pcb->quantum++;
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
	sem_t* semaphore_init = (sem_t*) list_get(g_scheduler_queues.exec_semaphores_init, exec_id);
	sem_t* semaphore_exec = (sem_t*) list_get(g_scheduler_queues.exec_semaphores, exec_id);

	while (true) {
		sem_wait(semaphore_init);
		exec_next_statement(exec_id);
		sem_wait(semaphore_exec);
	}
}

void exec_next_statement(int processor) {
	pcb_t* pcb = (pcb_t*) list_get(g_scheduler_queues.exec, processor);
	stats_t* event;
	statement_t* statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	// Aca se empieza la ejecucion del statement
	if (statement->operation <= INSERT) {
		// Si es select o insert nos interesa persistir informacion de la consulta
		event = malloc(sizeof(stats_t));
		event->timestamp_start = get_timestamp();
		event->memory = 0; // TODO: obtener en q memoria se ejecutaria esto
		event->event_type = statement->operation == SELECT ? SELECT_EVENT : INSERT_EVENT;
		pcb->last_execution_stats = event;
	}

	exec_remote(pcb, statement);
}

void exec_remote(pcb_t* pcb, statement_t* statement) {
	void* input = NULL;
	socket_operation_t network_operation;
	elements_network_t element_info;
	void (*callback)(void*, response_t*);

	//pcb_operation_t* pcb_operation = malloc(sizeof(pcb_operation_t));
	//char* demo_str = string_duplicate("soy un kernel");
	//do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, SELECT_IN, demo_str, 14, select_callback);
	//select_input_t* input = statement->select_input;
	// Esto deberia ir en el callback
	// Aca deberiamos tener el PCB
	switch (statement->operation) {
		case SELECT:
			network_operation = SELECT_IN;
			input = statement->select_input;
			element_info = elements_select_in_info(statement->select_input);
			callback = on_select;
		break;
		case INSERT:
			network_operation = INSERT_IN;
			input = statement->insert_input;
			element_info = elements_insert_in_info(statement->insert_input);
			callback = on_insert;
		break;
		case CREATE:
			network_operation = CREATE_IN;
			input = statement->create_input;
			element_info = elements_create_in_info(statement->create_input);
			callback = on_create;
		break;
		case DESCRIBE:
			network_operation = DESCRIBE_IN;
			input = statement->describe_input;
			element_info = elements_describe_in_info(statement->describe_input);
			callback = on_describe;
		break;
		case DROP:
			network_operation = DROP_IN;
			input = statement->drop_input;
			element_info = elements_drop_in_info(statement->drop_input);
			callback = on_drop;
		break;
		case JOURNAL:
			network_operation = JOURNAL_IN;
			element_info = elements_journal_in_info(NULL);
			callback = on_journal;
		break;
		default:
		break;
	}

	do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, network_operation, input,
			element_info.elements, element_info.elements_size, callback, true, NULL, pcb);

	sem_wait(statement->semaphore);

	// Sigue posterior al callback.
}

void on_select(void* result, response_t* response) {
	record_t* record = (record_t*) result;

	// Esto me da asco, pero bueno, perdoname diosito.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	if (record->timestamp == -2) {
		log_e("La tabla %s no existe. El SELECT ha fallado", record->table_name);
		pcb->errors = true;
	} else if (record->timestamp == -1) {
		log_e("La key solicitada no se encuentra en la tabla %s. El SELECT ha fallado", record->table_name);
		pcb->errors = true;
	} else {
		log_i("SELECT %i FROM %s: %s", record->key, record->table_name, record->value);
	}

	pcb->last_execution_stats->timestamp_end = get_timestamp();
	log_t("Se ingresa un evento a las estadisticas.");
	list_add(g_stats_events, pcb->last_execution_stats);
	clear_old_stats();

	post_exec_statement(pcb, current_statement);
}

void on_insert(void* result, response_t* response) {
	int* status = (int*) result;

	// Esto me da asco, pero bueno, perdoname diosito x2.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	// TODO: logica del insert aca
	// Debajo deberia ir lo del insert
	log_i("Recibi un %i pero la verdad no se que hacer con el. TODO: mostrar mensaje como la gente", *status);


	pcb->last_execution_stats->timestamp_end = get_timestamp();
	log_t("Se ingresa un evento a las estadisticas.");
	list_add(g_stats_events, pcb->last_execution_stats);
	clear_old_stats();

	post_exec_statement(pcb, current_statement);
}

void on_create(void* result, response_t* response) {
	int* status = (int*) result;
	create_input_t* input;

	// Esto me da asco, pero bueno, perdoname diosito x3.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);
	input = current_statement->create_input;

	if (*status == -2) {
		log_e("No hay bloques suficientes para crear la tabla %s con %i particiones. El CREATE ha fallado", input->table_name, input->partitions);
		pcb->errors = true;
	} else if (*status == -1) {
		log_e("La tabla %s ya existe en el sistema. El CREATE ha fallado", input->table_name);
		pcb->errors = true;
	} else {
		log_i("Se creo la tabla %s satisfactoriamente", input->table_name);
	}

	post_exec_statement(pcb, current_statement);
}

void on_describe(void* result, response_t* response) {
	t_list* metadata_list = (t_list*) result;

	// Esto me da asco, pero bueno, perdoname diosito x4.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	if (list_size(metadata_list) == 0) {
		log_i("No se encontraron tablas al hacer el DESCRIBE.");
	} else {
		for (int i = 0; i < list_size(metadata_list); i++) {
			table_metadata_t* metadata = (table_metadata_t*) list_get(metadata_list, i);
			char* consistency = get_consistency_name(metadata->consistency);
			log_i("Metadata de la tabla %s", metadata->table_name);
			log_i("  -> Tiempo de compactacion: %ld", metadata->compaction_time);
			log_i("  -> Consistencia: %s", consistency);
			log_i("  -> Cantidad de particiones: %i", metadata->partitions);
		}
	}

	post_exec_statement(pcb, current_statement);
}

void on_drop(void* result, response_t* response) {
	int* status = (int*) result;

	// Esto me da asco, pero bueno, perdoname diosito x5.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	// TODO: logica del drop aca
	// Debajo deberia ir lo del drop
	log_i("Recibi un %i pero la verdad no se que hacer con el. TODO: mostrar mensaje como la gente", *status);

	post_exec_statement(pcb, current_statement);
}

void on_journal(void* result, response_t* response) {
	int* status = (int*) result;

	// Esto me da asco, pero bueno, perdoname diosito x6.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	// TODO: logica del journal aca
	// Debajo deberia ir lo del journal
	log_i("Recibi un %i pero la verdad no se que hacer con el. TODO: mostrar mensaje como la gente", *status);

	post_exec_statement(pcb, current_statement);
}

void post_exec_statement(pcb_t* pcb, statement_t* current_statement) {
	if (pcb->errors) {
		log_e("Hubo un error al ejecutar un statement. Se cancela ejecucion del proceso con PID %i", pcb->process_id);
		pcb->program_counter = list_size(pcb->statements);
	} else {
		pcb->program_counter++;
	}

	sem_post(current_statement->semaphore);
	sem_post((sem_t*) list_get(g_scheduler_queues.exec_semaphores, pcb->processor));
}
