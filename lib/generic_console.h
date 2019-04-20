#ifndef GENERIC_CONSOLE_H_
#define GENERIC_CONSOLE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "generic_logger.h"
#include "commons/string.h"
#include "commons/log.h"

#include "utils/operation_types.h"
#include "utils/string.h"

typedef struct {
	void (*select)(select_input_t*);
	void (*insert)(insert_input_t*);
	void (*create)(create_input_t*);
	void (*describe)(describe_input_t*);
	void (*drop)(drop_input_t*);
	void (*journal)();
	void (*add)(add_input_t*);
	void (*run)(run_input_t*);
	void (*metrics)();
} callbacks_t;

void init_console(char* init_str, char* prefix, process_t type, callbacks_t* callbacks);
void process_command(char* command, process_t process, callbacks_t* callbacks);
operation_t get_operation(char* input);
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
bool validate_metrics(int tokens_size, char** tokens);
void process_input(operation_t operation, char* user_input, callbacks_t* callbacks);
callbacks_t* get_input_callbacks(void (*select)(select_input_t*), void (*insert)(insert_input_t*), void (*create)(create_input_t*), void (*describe)(describe_input_t*),
		void (*drop)(drop_input_t*), void (*journal)(), void (*add)(add_input_t*), void (*run)(run_input_t*), void (*metrics)());

callbacks_t* g_callbacks;

#endif /* GENERIC_CONSOLE_H_ */
