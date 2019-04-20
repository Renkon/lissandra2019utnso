#include "operations.h"

void process_select(select_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	statement->operation = SELECT;
	statement->select_input = malloc(sizeof(select_input_t));
	memcpy(statement->select_input, input, sizeof(select_input_t));

	//char* demo_str = string_duplicate("soy un kernel");
	//do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, SELECT_IN, demo_str, 14, select_callback);
	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_insert(insert_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	statement->operation = INSERT;
	statement->insert_input = malloc(sizeof(insert_input_t));
	memcpy(statement->insert_input, input, sizeof(insert_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_create(create_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	statement->operation = CREATE;
	statement->create_input = malloc(sizeof(create_input_t));
	memcpy(statement->create_input, input, sizeof(create_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_describe(describe_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	statement->operation = DESCRIBE;
	statement->describe_input = malloc(sizeof(describe_input_t));
	memcpy(statement->describe_input, input, sizeof(describe_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_drop(drop_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	statement->operation = DROP;
	statement->drop_input = malloc(sizeof(drop_input_t));
	memcpy(statement->drop_input, input, sizeof(drop_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_journal() {
	// TODO: invocar a una memoria para hacer journaling
}

void process_add(add_input_t* input) {
	// TODO: hacer lo del add a un criterio
}

void process_run(run_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	bool success = on_inner_run_request(pcb->statements, input, false);

	if (!success)
		delete_pcb(pcb);
	else
		list_add(g_scheduler_queues.new, pcb);
}

void process_metrics() {
	// TODO: metricas
}

bool on_inner_run_request(t_list* statements, run_input_t* input, bool free_input) {
	bool success = true;
	t_list* file_lines = get_file_lines(input->path);

	if (file_lines == NULL) {
		log_e("No se pudo abrir el archivo %s. No se ejecutara el RUN");
		return false;
	}

	if (list_is_empty(file_lines)) {
		log_w("No hay lineas para ejecutar en el archivo");
		return false;
	}

	for(int i = 0; i < list_size(file_lines); i++) {
		char* command = list_get(file_lines, i);
		operation_t operation = get_operation(command);
		if (operation == INVALID || !operation_allowed(operation, KERNEL) || !validate_input(operation, command)) {
			log_e("Script invalido. Sintaxis invalida. %s:%i > %s", input->path, (i + 1), command);
			success = false;
			break;
		}

		if (operation != RUN && operation > DROP) {
			log_e("Script invalido. Comando no habilitado en script. %s:%i > %s", input->path, (i + 1), command);
			success = false;
			break;
		}

		add_statement(statements, operation, command);
	}

	for (int i = 0; i < list_size(file_lines); i++) {
		free(list_get(file_lines, i));
		list_destroy(file_lines);
	}

	if (free_input)
		free(input);

	return success;
}

void add_statement(t_list* statements, operation_t operation, char* command) {
	int tokens_size;
	char** tokens = string_split_ignore_quotes(command, " ", &tokens_size);
	statement_t* statement = malloc(sizeof(statement_t));
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;
	run_input_t* run_input;

	statement->operation = operation;

	switch(operation) {
		case SELECT:
			select_input = malloc(sizeof(select_input_t));

			select_input->table_name = malloc(strlen(tokens[1]) + 1);
			select_input->table_name = memcpy(select_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			select_input->key = string_to_uint16(tokens[2]);

			statement->select_input = select_input;
		break;
		case INSERT:
			insert_input = malloc(sizeof(insert_input_t));

			insert_input->table_name = malloc(strlen(tokens[1]) + 1);
			insert_input->table_name = memcpy(insert_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			insert_input->key = string_to_uint16(tokens[2]);
			insert_input->value = malloc(strlen(tokens[3]) + 1);
			insert_input->value = memcpy(insert_input->value, tokens[3], strlen(tokens[3]) + 1);
			if (tokens_size == 5)
				insert_input->timestamp = string_to_long(tokens[4]);
			else
				insert_input->timestamp = -1;

			statement->insert_input = insert_input;
		break;
		case CREATE:
			create_input = malloc(sizeof(create_input_t));

			create_input->table_name = malloc(strlen(tokens[1]) + 1);
			create_input->table_name = memcpy(create_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			if (string_equals_ignore_case(tokens[2], "SC"))
				create_input->consistency = STRONG_CONSISTENCY;
			else if (string_equals_ignore_case(tokens[2], "SHC"))
				create_input->consistency = STRONG_HASH_CONSISTENCY;
			else
				create_input->consistency = EVENTUAL_CONSISTENCY;
			create_input->partitions = string_to_int(tokens[3]);
			create_input->compaction_time = string_to_long(tokens[4]);

			statement->create_input = create_input;
		break;
		case DESCRIBE:
			describe_input = malloc(sizeof(describe_input_t));
			if (tokens_size == 2) {
				describe_input->table_name = malloc(strlen(tokens[1]) + 1);
				describe_input->table_name = memcpy(describe_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			} else {
				describe_input->table_name = string_new();
			}

			statement->describe_input = describe_input;
		break;
		case DROP:
			drop_input = malloc(sizeof(drop_input_t));

			drop_input->table_name = malloc(strlen(tokens[1]) + 1);
			drop_input->table_name = memcpy(drop_input->table_name, tokens[1], strlen(tokens[1]) + 1);

			statement->drop_input = drop_input;
		break;
		case RUN:
			run_input = malloc(sizeof(run_input_t));

			run_input->path = malloc(strlen(tokens[1]) + 1);
			run_input->path = memcpy(run_input->path, tokens[1], strlen(tokens[1]) + 1);

			on_inner_run_request(statements, run_input, true);
			return;
		break;
		default:
		break;
	}

	list_add(statements, statement);
}

