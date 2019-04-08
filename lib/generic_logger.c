#include "generic_logger.h"

#define log_generic_template(log_func, orig_log_func)        \
        void log_func(const char* msg, ...) {                \
            va_list args;                                    \
            char* msg_formatted;                             \
            va_start(args, msg);                             \
            msg_formatted = string_from_vformat(msg, args);  \
            orig_log_func(logger, msg_formatted);            \
            free(msg_formatted);                             \
            va_end(args);                                    \
        }

bool init_logger(char* file, char* program, bool print_console, t_log_level log_level){
	logger = log_create(file, program, print_console, log_level);

	if (logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		perror("No se pudo configurar logger. Abortando ejecucion");
		return false;
	}

	return true;
}

void destroy_logger() {
	log_destroy(logger);

}

log_generic_template(log_t, log_trace);

log_generic_template(log_d, log_debug);

log_generic_template(log_i, log_info);

log_generic_template(log_w, log_warning);

log_generic_template(log_e, log_error);
