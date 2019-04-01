#ifndef GENERIC_CONFIG_H_
#define GENERIC_CONFIG_H_

#include "commons/config.h"
#include "commons/log.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char* config_file;
	void (*callback_created)(t_config*);
	void (*callback_updated)(t_config*);
	char** config_keys;
	int config_size;
	t_log* logger;
} config_args_t;

void pre_init_config(config_args_t* args);
void init_config(char* config_file, void (*callback_created)(t_config*), void (*callback_updated)(t_config*), char** config_keys, int config_size, t_log* logger);
void update_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger);
void check_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger);
bool validate_config(t_config* config, char** config_keys, int config_size, t_log* logger);
bool validate_config_properties(t_config* config, char** config_keys, int config_size, t_log* logger);
config_args_t* build_config_args(char** config_keys, int config_size, char* file, t_log* logger, void (*created)(t_config*), void (*updated)(t_config*));

#endif /* GENERIC_CONFIG_H_ */
