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
	statement_t* statement = (statement_t*) list_get(pcb->statements, pcb->program_counter++);

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
	//void (*callback)(int, void*, bool);
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
			//callback = on_select;
		break;
		case INSERT:
			network_operation = INSERT_IN;
			input = statement->insert_input;
			//callback = on_insert;
		break;
		case CREATE:
			network_operation = CREATE_IN;
			input = statement->create_input;
			//callback = on_create;
		break;
		case DESCRIBE:
			network_operation = DESCRIBE_IN;
			input = statement->describe_input;
			//callback = on_describe;
		break;
		case DROP:
			network_operation = DROP_IN;
			input = statement->drop_input;
			//callback = on_drop;
		break;
		case JOURNAL:
			network_operation = JOURNAL_IN;
			//callback = on_journal;
		break;
		default:
		break;
	}

	do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, network_operation, input, get_input_size(statement->operation, input), /* CALLBACK */ NULL, true);

	if (statement->operation <= INSERT) {
		pcb->last_execution_stats->timestamp_end = get_timestamp();
		log_t("Se ingresa un evento a las estadisticas.");
		clear_old_stats();
		list_add(g_stats_events, pcb->last_execution_stats);
	}

	if (pcb->errors) {
		log_e("Hubo un error al ejecutar un statement. Se cancela ejecucion del proceso con PID %i", pcb->process_id);
		pcb->program_counter = list_size(pcb->statements);
	}

	pcb->last_execution_stats->timestamp_end = get_timestamp();
	log_t("Se ingresa un evento a las estadisticas.");
	clear_old_stats();
	list_add(g_stats_events, pcb->last_execution_stats);

	sem_post((sem_t*) list_get(g_scheduler_queues.exec_semaphores, pcb->processor));
}

void on_statement_failure(pcb_t* pcb) {
	pcb->errors = true;
}

int get_input_size(operation_t operation, void* input) {
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;

	switch (operation) {
		case SELECT:
			select_input = (select_input_t*) input;
			return strlen(select_input->table_name) + sizeof(uint16_t) + 1;
		break;
		case INSERT:
			insert_input = (insert_input_t*) input;
			return sizeof(uint16_t) + sizeof(long long) + strlen(insert_input->table_name) + strlen(insert_input->value) + 2;
		break;
		case CREATE:
			create_input = (create_input_t*) input;
			return sizeof(consistency_t) + sizeof(int) + sizeof(long) + strlen(create_input->table_name) + 1;
		break;
		case DESCRIBE:
			describe_input = (describe_input_t*) input;
			int describe_length = strlen(describe_input->table_name);
			return describe_length + (describe_length == 0) ? 0 : 1;
		break;
		case DROP:
			drop_input = (drop_input_t*) input;
			return strlen(drop_input->table_name) + 1;
		break;
		default:
		break;
	}

	return 0;
}
