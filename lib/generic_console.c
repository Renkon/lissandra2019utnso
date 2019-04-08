#include "generic_console.h"

void init_console(char* init_str, char* prefix, process_t type, t_log* logger) {
	char* user_input = NULL;
	operation_t operation = INVALID;

	// Mostramos el titulo del programa
	puts(init_str);

	while (true) {
		user_input = readline(prefix);

		operation = get_operation(user_input, type);

		if (operation == INVALID || !operation_allowed(operation, type) || !validate_input(operation, user_input)) {
			log_error(logger, "Se solicito una operacion invalida: \"%s\"", user_input);
			puts("Operacion invalida. Revise su input");
		} else {

		}

		free(user_input);
		user_input = NULL;
		operation = INVALID;
	}
}

// En base al token inicial devuelve el tipo de operacion
operation_t get_operation(char* input, process_t process_type) {
	operation_t operation_type;
	char** tokens = string_split(input, " ");

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
	else if (string_equals_ignore_case(tokens[0], "ADD") && string_equals_ignore_case(tokens[1], "MEMORY"))
		operation_type = ADD;
	else if (string_equals_ignore_case(tokens[0], "RUN"))
		operation_type = RUN;
	else
		operation_type = INVALID;

	free(tokens);

	return operation_type;
}

bool operation_allowed(operation_t operation, process_t process) {
	if (process == FILESYSTEM)
		return operation <= DROP;
	if (process == MEMORY)
		return operation <= ADD;

	// El kernel acepta todos
	return true;
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
	if (end_str[0] != '\0' || value < 0 || value > 65535)
		return false;

	return true;
}

bool validate_insert(int tokens_size, char** tokens) {
	char* end_str;
	long value_key;
	long value_timestamp;

	//  INSERT [NOMBRE_TABLA] [KEY] "[VALOR]" [TIMESTAMP]*
	if (tokens_size != 4 && tokens_size != 5)
		return false;

	// Valido que la key sea un uint16_t (si no que explote)
	value_key = strtol(tokens[2], &end_str, 10);
		if (end_str[0] != '\0' || value_key < 0 || value_key > 65535)
			return false;

	if (tokens_size == 5) { // Valido el timestamp
		value_timestamp = strtol(tokens[4], &end_str, 10);
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

	return string_equals_ignore_case(tokens[3], "TO");

}

bool validate_run(int tokens_size, char** tokens) {
	// RUN [ARCHIVO]
	return tokens_size == 2;
}
