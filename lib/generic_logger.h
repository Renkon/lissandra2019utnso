#ifndef GENERIC_LOGGER_H_
#define GENERIC_LOGGER_H_

#include <stdarg.h>
#include <stdlib.h>
#include "commons/log.h"
#include "commons/string.h"

t_log* logger;

bool init_logger(char* file, char* program, bool print_console, t_log_level log_level);
void destroy_logger();
void log_t(const char* msg, ...);
void log_d(const char* msg, ...);
void log_i(const char* msg, ...);
void log_w(const char* msg, ...);
void log_e(const char* msg, ...);

#endif /* GENERIC_LOGGER_H_ */
