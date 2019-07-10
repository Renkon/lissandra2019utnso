#include "operations.h"

void process_select(select_input_t* input, response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = SELECT;
	statement->select_input = malloc(sizeof(select_input_t));
	statement->select_input->table_name = malloc(strlen(input->table_name) + 1);

	statement->select_input->key = input->key;
	memcpy(statement->select_input->table_name, input->table_name, strlen(input->table_name) + 1);

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_insert(insert_input_t* input, response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = INSERT;
	statement->insert_input = malloc(sizeof(insert_input_t));
	statement->insert_input->table_name = malloc(strlen(input->table_name) + 1);
	statement->insert_input->value = malloc(strlen(input->value) + 1);

	statement->insert_input->key = input->key;
	statement->insert_input->timestamp = input->timestamp;
	memcpy(statement->insert_input->table_name, input->table_name, strlen(input->table_name) + 1);
	memcpy(statement->insert_input->value, input->value, strlen(input->value) + 1);

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_create(create_input_t* input, response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = CREATE;
	statement->create_input = malloc(sizeof(create_input_t));
	statement->create_input->table_name = malloc(strlen(input->table_name) + 1);

	statement->create_input->compaction_time = input->compaction_time;
	statement->create_input->consistency = input->consistency;
	statement->create_input->partitions = input->partitions;
	memcpy(statement->create_input->table_name, input->table_name, strlen(input->table_name) + 1);

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_describe(describe_input_t* input, response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = DESCRIBE;
	statement->describe_input = malloc(sizeof(describe_input_t));

	if (input->table_name != NULL) {
		statement->describe_input->table_name = malloc(strlen(input->table_name) + 1);
		memcpy(statement->describe_input->table_name, input->table_name, strlen(input->table_name) + 1);
	} else {
		statement->describe_input->table_name = NULL;
	}

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_drop(drop_input_t* input, response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = DROP;
	statement->drop_input = malloc(sizeof(drop_input_t));
	statement->drop_input->table_name = malloc(strlen(input->table_name) + 1);

	memcpy(statement->drop_input->table_name, input->table_name, strlen(input->table_name) + 1);

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_journal(response_t* response) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = generate_statement();

	statement->operation = JOURNAL;

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
	sem_post(&g_lts_semaphore);
}

void process_add(add_input_t* input) {
	switch (input->consistency) {
		case STRONG_CONSISTENCY:
			add_sc_memory(input->memory_number);
		break;
		case STRONG_HASH_CONSISTENCY:
			add_shc_memory(input->memory_number);
		break;
		case EVENTUAL_CONSISTENCY:
			add_ec_memory(input->memory_number);
		break;
	}
}

statement_t* generate_statement() {
	statement_t* statement = malloc(sizeof(statement_t));
	statement->create_input = NULL;
	statement->describe_input = NULL;
	statement->drop_input = NULL;
	statement->insert_input = NULL;
	statement->select_input = NULL;
	statement->semaphore = malloc(sizeof(sem_t));
	sem_init(statement->semaphore, 0, 0);
	return statement;
}

void process_run(run_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	bool success = on_inner_run_request(pcb->statements, input, false);

	if (!success) {
		delete_pcb(pcb);
	}
	else {
		list_add(g_scheduler_queues.new, pcb);
		sem_post(&g_lts_semaphore);
	}
}

bool on_inner_run_request(t_list* statements, run_input_t* input, bool free_input) {
	bool success = true;
	t_list* file_lines = get_file_lines(input->path);

	if (file_lines == NULL) {
		log_e("No se pudo abrir el archivo %s. No se ejecutara el RUN", input->path);
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

		if (operation != RUN && operation > JOURNAL) {
			log_e("Script invalido. Comando no habilitado en script. %s:%i > %s", input->path, (i + 1), command);
			success = false;
			break;
		}

		add_statement(statements, operation, command);
	}

	for (int i = 0; i < list_size(file_lines); i++) {
		free(list_get(file_lines, i));
	}
	list_destroy(file_lines);

	if (free_input)
		free(input);

	return success;
}

void add_statement(t_list* statements, operation_t operation, char* command) {
	int tokens_size;
	bool run = false;
	char** tokens = string_split_ignore_quotes(command, " ", &tokens_size);
	statement_t* statement = generate_statement();
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
				insert_input->timestamp = string_to_long_long(tokens[4]);
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
				describe_input->table_name = NULL;
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
			run = true;
		break;
		default:
		break;
	}

	if (!run)
		list_add(statements, statement);

	for (int i = 0; i < tokens_size; i++)
		free(tokens[i]);
	free(tokens);
}

