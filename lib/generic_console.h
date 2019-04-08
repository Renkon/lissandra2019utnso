#ifndef GENERIC_CONSOLE_H_
#define GENERIC_CONSOLE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "commons/string.h"
#include "commons/log.h"
#include "utils/string.h"

typedef enum {
	INVALID,
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP, // hasta aca puede el fs
	JOURNAL, // hasta aca puede la memoria
	ADD,
	RUN // y hasta aca puede el kernel
} operation_t;

typedef enum {
	KERNEL,
	FILESYSTEM,
	MEMORY
} process_t;

void init_console(char* init_str, char* prefix, process_t type, t_log* logger);
operation_t get_operation(char* input, process_t type);
bool operation_allowed(operation_t operation, process_t process);
bool validate_input(operation_t operation, char* input);
bool validate_select(int tokens_size, char** tokens);
bool validate_insert(int tokens_size, char** tokens);
bool validate_create(int tokens_size, char** tokens);
bool validate_describe(int tokens_size, char** tokens);
bool validate_drop(int tokens_size, char** tokens);
bool validate_journal(int tokens_size, char** tokens);
bool validate_add(int tokens_size, char** tokens);
bool validate_run(int tokens_size, char** tokens);

#endif /* GENERIC_CONSOLE_H_ */
