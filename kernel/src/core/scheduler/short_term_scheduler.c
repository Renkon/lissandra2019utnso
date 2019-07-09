#include "short_term_scheduler.h"

void short_term_schedule() {
	pcb_t* pcb;
	sem_t* semaphore_init;

	sem_init(&g_sts_semaphore, 0, 0);

	__pcbs_to_ready = list_create();

	for (int i = 0; i < list_size(g_scheduler_queues.exec); i++)
		list_add(__pcbs_to_ready, NULL);

	pthread_detach(pthread_self());

	// Este bloque se ejecuta continuamente, y es la mente maestra del planificador
	while (true) {
		sem_wait(&g_sts_semaphore);
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

					log_d("Proceso %i pasado a Exec en hilo %i", pcb->process_id, i);
					pcb->__recently_exec = true;
					sem_post_neg(&g_sts_semaphore);
				}
			} else if (pcb->__recently_exec) {
				semaphore_init = (sem_t*) list_get(g_scheduler_queues.exec_semaphores_init, i);
				pcb->processor = i;
				sem_post(semaphore_init);
				// Aca el planificador ejecuta
				pcb->quantum++;
				pcb->__recently_exec = false;
			}
		}

		for (int i = 0; i < list_size(__pcbs_to_ready); i++) {
			pcb_t* pcb = (pcb_t*) list_get(__pcbs_to_ready, i);
			if (pcb != NULL && pcb->__recently_ready) {
				pcb->__recently_ready = false;
				list_replace(__pcbs_to_ready, i, NULL);
				sem_post_neg(&g_sts_semaphore);
			}
		}

	}
}

void planifier_execute(void* arg) {
	pthread_detach(pthread_self());

	pcb_t* pcb;
	int exec_id = *((int*) arg);
	sem_t* semaphore_init = (sem_t*) list_get(g_scheduler_queues.exec_semaphores_init, exec_id);
	sem_t* semaphore_exec = (sem_t*) list_get(g_scheduler_queues.exec_semaphores, exec_id);

	while (true) {
		sem_wait(semaphore_init);
		exec_next_statement(exec_id);
		sem_wait(semaphore_exec);

		// Revisamos si termino de ejecutar
		pcb = (pcb_t*) list_get(g_scheduler_queues.exec, exec_id);
		if (pcb->program_counter >= list_size(pcb->statements)) {
			list_remove(g_scheduler_queues.exec, exec_id);
			list_add(g_scheduler_queues.exit, pcb);
			list_add_in_index(g_scheduler_queues.exec, exec_id, NULL);
			pcb->state = EXIT;

			log_d("Proceso %i finalizo su ejecucion", pcb->process_id);
			list_replace(__pcbs_to_ready, exec_id, NULL);
			sem_post(&g_lts_semaphore);
			sem_post_neg(&g_sts_semaphore);
		} else if (pcb->quantum >= g_config.quantum) {
			// Termino su ciclo de quantum
			list_remove(g_scheduler_queues.exec, exec_id);
			list_add(g_scheduler_queues.ready, pcb);
			list_replace(__pcbs_to_ready, exec_id, pcb);
			pcb->state = READY;
			pcb->__recently_ready = true;
			list_add_in_index(g_scheduler_queues.exec, exec_id, NULL);

			log_d("Proceso %i vuelve a cola de listos", pcb->process_id);
			sem_post_neg(&g_sts_semaphore);
		} else {
			semaphore_init = (sem_t*) list_get(g_scheduler_queues.exec_semaphores_init, exec_id);
			pcb->processor = exec_id;
			usleep(g_config.execution_delay * 1000);
			sem_post(semaphore_init);
			// Aca el planificador ejecuta
			pcb->quantum++;
		}
	}
}

void sem_post_neg(sem_t* semaphore) {
	int sem;
	sem_getvalue(semaphore, &sem);
	if (sem <= 0)
		sem_post(semaphore);
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

	// Toda esta asquerosidad es para validar que este en el describe la tabla que queremos usar.
	if ((statement->operation == SELECT && !table_exists_in_metadata(statement->select_input->table_name))
		|| (statement->operation == INSERT && !table_exists_in_metadata(statement->insert_input->table_name))
		|| (statement->operation == DROP && !table_exists_in_metadata(statement->drop_input->table_name))) {
		char* table_name;

		free(element_info.elements_size);

		if (statement->operation == SELECT)
			table_name = statement->select_input->table_name;
		else if (statement->operation == INSERT)
			table_name = statement->insert_input->table_name;
		else
			table_name = statement->drop_input->table_name;

		if (statement->operation <= INSERT) {
			pcb->last_execution_stats->timestamp_end = get_timestamp();
			log_t("Se ingresa un evento a las estadisticas.");
			list_add(g_stats_events, pcb->last_execution_stats);
			clear_old_stats();
		}

		log_e("No se pudo realizar la operacion con la tabla %s. No esta en la metadata de tablas.", table_name);
		log_e("Hubo un error al ejecutar un statement. Se cancela ejecucion del proceso con PID %i", pcb->process_id);

		pcb->errors = true;
		pcb->program_counter = list_size(pcb->statements);
		sem_post((sem_t*) list_get(g_scheduler_queues.exec_semaphores, pcb->processor));
	} else {
		// TODO: agregar la wea de mandarle solo a la memoria asignada!
		do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, network_operation, input,
				element_info.elements, element_info.elements_size, callback, true, NULL, pcb);

		sem_wait(statement->semaphore);

		// Sigue posterior al callback.
	}
}

void on_select(void* result, response_t* response) {
	record_t* record = (record_t*) result;

	// Esto me da asco, pero bueno, perdoname diosito.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	if (record == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El SELECT ha fallado");
		pcb->errors = true;
	} else if (record->timestamp == -4) {
		log_e("La memoria asignada no pudo realizar un journaling para seleccionar un valor. El SELECT ha fallado");
		pcb->errors = true;
	} else if (record->timestamp == -3) {
		log_e("Hubo un error de red al querer ir a buscar un valor al FS. El SELECT ha fallado");
		pcb->errors = true;
	} else if (record->timestamp == -2) {
		log_i("La tabla %s no existe. El SELECT ha fallado", record->table_name);
		pcb->errors = true;
	} else if (record->timestamp == -1) {
		log_i("SELECT FROM %s no tiene valor", record->table_name);
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
	insert_input_t* input = current_statement->insert_input;

	// TODO: logica del insert aca
	// Debajo deberia ir lo del insert
	if (status == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El INSERT ha fallado");
		pcb->errors = true;
	} else if (*status == -4) {
		log_e("La memoria no pudo realizar el insert dado que tenia la memoria llena y fallo el journaling. EL INSERT ha fallado");
		pcb->errors = true;
	} else if (*status == -1) {
		log_w("El tamaño del valor (%s) del registro insertado es mayor al permitido. EL INSERT ha fallado", input->value);
	} else {
		log_i("Se inserto en %s:%i. Valor: %s", input->table_name, input->key, input->value);
	}

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
	if (status == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El CREATE ha fallado");
		pcb->errors = true;
	} else if (*status == -3) {
		log_e("Hubo un error de red al querer crear la tabla %s. El CREATE ha fallado", input->table_name);
		pcb->errors = true;
	} else if (*status == -2) {
		log_e("No hay bloques suficientes para crear la tabla %s con %i particiones. El CREATE ha fallado", input->table_name, input->partitions);
		pcb->errors = true;
	} else if (*status == -1) {
		log_e("La tabla %s ya existe en el sistema. El CREATE ha fallado", input->table_name);
		pcb->errors = true;
	} else {
		log_i("Se creo la tabla %s satisfactoriamente", input->table_name);
		on_post_create(input);
	}

	post_exec_statement(pcb, current_statement);
}

void on_describe(void* result, response_t* response) {
	t_list* metadata_list = (t_list*) result;

	// Esto me da asco, pero bueno, perdoname diosito x4.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	if (metadata_list == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El DESCRIBE ha fallado");
		pcb->errors = true;
	} else {
		if (list_size(metadata_list) == 0) {
			log_w("No se encontraron tablas al hacer el DESCRIBE.");
			log_w("Puede ser que la tabla no exista, o que haya fallado la solicitud al filesystem");
		} else {
			for (int i = 0; i < list_size(metadata_list); i++) {
				table_metadata_t* metadata = (table_metadata_t*) list_get(metadata_list, i);
				char* consistency = get_consistency_name(metadata->consistency);
				log_i("Metadata de la tabla %s", metadata->table_name);
				log_i("  -> Tiempo de compactacion: %ld", metadata->compaction_time);
				log_i("  -> Consistencia: %s", consistency);
				log_i("  -> Cantidad de particiones: %i", metadata->partitions);
			}

			on_post_describe(metadata_list, list_size(metadata_list) == 1, true);
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
	if (status == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El DROP ha fallado");
		pcb->errors = true;
	} else if (*status == -2) {
		log_e("Hubo un error de red al querer droppear la tabla %s. El DROP ha fallado", current_statement->drop_input->table_name);
		pcb->errors = true;
	} else if (*status == -1) {
		log_e("La tabla %s no existe. El DROP ha fallado", current_statement->drop_input->table_name);
		pcb->errors = true;
	} else {
		log_i("Se dropeo la tabla %s", current_statement->drop_input->table_name);
		on_post_drop(current_statement->drop_input);
	}

	post_exec_statement(pcb, current_statement);
}

void on_journal(void* result, response_t* response) {
	int* status = (int*) result;

	// Esto me da asco, pero bueno, perdoname diosito x6.
	pcb_t* pcb = (pcb_t*) response;
	statement_t* current_statement = (statement_t*) list_get(pcb->statements, pcb->program_counter);

	// TODO: logica del journal aca
	// Debajo deberia ir lo del journal
	if (status == NULL) {
		log_e("Hubo un error de red al querer comunicarme con la memoria asignada. El INSERT ha fallado");
		pcb->errors = true;
	} else {
		log_i("Recibi un %i pero la verdad no se que hacer con el. TODO: mostrar mensaje como la gente", *status);
	}

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
