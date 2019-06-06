#include "generic_console.h"

void init_console(char* init_str, char* prefix, process_t type, callbacks_t* callbacks) {
	char* user_input = NULL;

	// Mostramos el titulo del programa
	puts(init_str);

	while (true) {
		user_input = readline(prefix);

		process_command(user_input, type, callbacks);

		free(user_input);
		user_input = NULL;
	}
}

bool process_command(char* command, process_t process, callbacks_t* callbacks) {
	operation_t operation = get_operation(command);

	if (operation == INVALID || !operation_allowed(operation, process) || !validate_input(operation, command)) {
		log_e("Se solicito una operacion invalida: \"%s\"", command);
		puts("Operacion invalida. Revise su input");
		return false;
	} else {
		process_input(operation, command, callbacks);
		return true;
	}
}

// En base al token inicial devuelve el tipo de operacion
operation_t get_operation(char* input) {
	operation_t operation_type;
	int tokens_size;
	char** tokens = string_split_ignore_quotes(input, " ", &tokens_size);

	if (tokens_size == 0)
		return INVALID;

	if (string_equals_ignore_case(tokens[0], "SELECT"))
		operation_type = SELECT;
	else if (string_equals_ignore_case(tokens[0], "INSERT"))
		operation_type = INSERT;
	else if (string_equals_ignore_case(tokens[0], "CREATE"))
		operation_type = CREATE;
	else if (string_equals_ignore_case(tokens[0], "DESCRIBE"))
		operation_type = DESCRIBE;
	else if (string_equals_ignore_case(tokens[0], "DROP"))
		operation_type = DROP;
	else if (string_equals_ignore_case(tokens[0], "JOURNAL"))
		operation_type = JOURNAL;
	else if (tokens_size > 1 && string_equals_ignore_case(tokens[0], "ADD") && string_equals_ignore_case(tokens[1], "MEMORY"))
		operation_type = ADD;
	else if (string_equals_ignore_case(tokens[0], "RUN"))
		operation_type = RUN;
	else if (string_equals_ignore_case(tokens[0], "METRICS"))
		operation_type = METRICS;
	else
		operation_type = INVALID;

	for (int i = 0; i < tokens_size; i++)
		free(tokens[i]);
	free(tokens);

	return operation_type;
}

bool operation_allowed(operation_t operation, process_t process) {
	if (process == FILESYSTEM)
		return operation <= DROP;
	if (process == MEMORY)
		return operation <= JOURNAL;

	return process == KERNEL;
}

bool validate_input(operation_t operation, char* input) {
	int tokens_size;
	char** tokens = string_split_ignore_quotes(input, " ", &tokens_size);
	bool result = false;

	switch (operation) {
		case SELECT:
			result = validate_select(tokens_size, tokens);
		break;
		case INSERT:
			result = validate_insert(tokens_size, tokens);
		break;
		case CREATE:
			result = validate_create(tokens_size, tokens);
		break;
		case DESCRIBE:
			result = validate_describe(tokens_size, tokens);
		break;
		case DROP:
			result = validate_drop(tokens_size, tokens);
		break;
		case JOURNAL:
			result = validate_journal(tokens_size, tokens);
		break;
		case ADD:
			result = validate_add(tokens_size, tokens);
		break;
		case RUN:
			result = validate_run(tokens_size, tokens);
		break;
		case METRICS:
			result = validate_metrics(tokens_size, tokens);
		break;
		// Esto no pasa nunca (pero evito un warning del eclipse)
		default:
		break;
	}

	for (int i = 0; i < tokens_size; i++) {
		free(tokens[i]);
	}
	free(tokens);
	return result;
}

bool validate_select(int tokens_size, char** tokens) {
	char* end_str;
	long value;

	// SELECT [NOMBRE TABLA] [KEY]
	if (tokens_size != 3)
		return false;

	// Valido que la key sea un uint16_t (si no que explote)
	value = strtol(tokens[2], &end_str, 10);
	if (end_str[0] != '\0' || value < 0 || value > UINT16_MAX)
		return false;

	return true;
}

bool validate_insert(int tokens_size, char** tokens) {
	char* end_str;
	long value_key;
	long long value_timestamp;

	//  INSERT [NOMBRE_TABLA] [KEY] "[VALOR]" [TIMESTAMP]*
	if (tokens_size != 4 && tokens_size != 5)
		return false;

	// Valido que la key sea un uint16_t (si no que explote)
	value_key = strtol(tokens[2], &end_str, 10);
	if (end_str[0] != '\0' || value_key < 0 || value_key > UINT16_MAX)
		return false;

	if (tokens_size == 5) { // Valido el timestamp
		value_timestamp = strtoll(tokens[4], &end_str, 10);
		if (end_str[0] != '\0' || value_timestamp < 0)
			return false;
	}

	return true;
}

bool validate_create(int tokens_size, char** tokens) {
	char* end_str;
	long value_partitions;
	long value_compaction;

	// CREATE [NOMBRE_TABLA] [CRIT_CONSISTENCIA] [PARTICIONES] [TIEMPO_COMPACTACION]
	if (tokens_size != 5)
		return false;

	if (!string_equals_ignore_case(tokens[2], "SC") &&
		!string_equals_ignore_case(tokens[2], "SHC") &&
		!string_equals_ignore_case(tokens[2], "EC"))
		return false;

	value_partitions = strtol(tokens[3], &end_str, 10);
	if (end_str[0] != '\0' || value_partitions < 0)
		return false;

	value_compaction = strtol(tokens[4], &end_str, 10);
	if (end_str[0] != '\0' || value_compaction < 0)
		return false;

	return true;
}

bool validate_describe(int tokens_size, char** tokens) {
	// DESCRIBE o DESCRIBE [NOMBRE_TABLA]
	return tokens_size == 1 || tokens_size == 2;
}

bool validate_drop(int tokens_size, char** tokens) {
	// DROP [NOMBRE_TABLA]
	return tokens_size == 2;
}

bool validate_journal(int tokens_size, char** tokens) {
	// JOURNAL
	return tokens_size == 1;
}

bool validate_add(int tokens_size, char** tokens) {
	char* end_str;
	long value;

	// ADD MEMORY [NUM_MEMORIA] TO [CRIT_CONSISTENCIA]
	if (tokens_size != 5)
		return false;

	value = strtol(tokens[2], &end_str, 10);
	if (end_str[0] != '\0' || value < 0)
		return false;

	if (!string_equals_ignore_case(tokens[3], "TO"))
		return false;

	return string_equals_ignore_case(tokens[4], "SC") ||
		string_equals_ignore_case(tokens[4], "SHC") ||
		string_equals_ignore_case(tokens[4], "EC");
}

bool validate_run(int tokens_size, char** tokens) {
	// RUN [ARCHIVO]
	return tokens_size == 2;
}

bool validate_metrics(int tokens_size, char** tokens) {
	// METRICS
	return tokens_size == 1;
}

void process_input(operation_t operation, char* user_input, callbacks_t* callbacks) {
	int tokens_size;
	char** tokens = string_split_ignore_quotes(user_input, " ", &tokens_size);
	select_input_t* select_input;
	insert_input_t* insert_input;
	create_input_t* create_input;
	describe_input_t* describe_input;
	drop_input_t* drop_input;
	add_input_t* add_input;
	run_input_t* run_input;

	switch (operation) {
		case SELECT:
			select_input = malloc(sizeof(select_input_t));

			select_input->table_name = malloc(strlen(tokens[1]) + 1);
			select_input->table_name = memcpy(select_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			select_input->key = string_to_uint16(tokens[2]);

			callbacks->select(select_input);
			free(select_input->table_name);
			free(select_input);
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

			callbacks->insert(insert_input);
			free(insert_input->table_name);
			free(insert_input->value);
			free(insert_input);
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

			callbacks->create(create_input);
			free(create_input->table_name);
			free(create_input);
		break;
		case DESCRIBE:
			describe_input = malloc(sizeof(describe_input_t));
			if (tokens_size == 2) {
				describe_input->table_name = malloc(strlen(tokens[1]) + 1);
				describe_input->table_name = memcpy(describe_input->table_name, tokens[1], strlen(tokens[1]) + 1);
			} else {
				describe_input->table_name = NULL;
			}

			callbacks->describe(describe_input);
			if (tokens_size == 2)
				free(describe_input->table_name);
			free(describe_input);
		break;
		case DROP:
			drop_input = malloc(sizeof(drop_input_t));

			drop_input->table_name = malloc(strlen(tokens[1]) + 1);
			drop_input->table_name = memcpy(drop_input->table_name, tokens[1], strlen(tokens[1]) + 1);

			callbacks->drop(drop_input);
			free(drop_input->table_name);
			free(drop_input);
		break;
		case JOURNAL:
			callbacks->journal();
		break;
		case ADD:
			add_input = malloc(sizeof(add_input_t));

			add_input->memory_number = string_to_int(tokens[2]);
			if (string_equals_ignore_case(tokens[4], "SC"))
				add_input->consistency = STRONG_CONSISTENCY;
			else if (string_equals_ignore_case(tokens[4], "SHC"))
				add_input->consistency = STRONG_HASH_CONSISTENCY;
			else
				add_input->consistency = EVENTUAL_CONSISTENCY;

			callbacks->add(add_input);
			free(add_input);
		break;
		case RUN:
			run_input = malloc(sizeof(run_input_t));

			run_input->path = malloc(strlen(tokens[1]) + 1);
			run_input->path = memcpy(run_input->path, tokens[1], strlen(tokens[1]) + 1);

			callbacks->run(run_input);
			free(run_input->path);
			free(run_input);
		break;
		case METRICS:
			callbacks->metrics();
		break;
		// Esto no pasa nunca (pero evito un warning del eclipse)
		default:
		break;
	}

	for (int i = 0; i < tokens_size; i++) {
		free(tokens[i]);
	}
	free(tokens);
}

callbacks_t* get_input_callbacks(void (*select)(select_input_t*), void (*insert)(insert_input_t*), void (*create)(create_input_t*), void (*describe)(describe_input_t*),
		void (*drop)(drop_input_t*), void (*journal)(), void (*add)(add_input_t*), void (*run)(run_input_t*), void (*metrics)()){
	if (g_callbacks != NULL)
		return g_callbacks;

	g_callbacks = malloc(sizeof(callbacks_t));

	g_callbacks->select = select;
	g_callbacks->insert = insert;
	g_callbacks->create = create;
	g_callbacks->describe = describe;
	g_callbacks->drop = drop;
	g_callbacks->journal = journal;
	g_callbacks->add = add;
	g_callbacks->run = run;
	g_callbacks->metrics = metrics;

	return g_callbacks;
}
