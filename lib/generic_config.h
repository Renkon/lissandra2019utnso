#ifndef GENERIC_CONFIG_H_
#define GENERIC_CONFIG_H_
#define EVENT_SIZE    ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

#include "commons/config.h"
#include "commons/collections/list.h"
#include "commons/log.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	char* config_file;
	void (*callback_updated)(t_config*);
	char** config_keys;
	int config_size;
	t_log* logger;
} config_args_t;

bool init_config(char* config_file, void (*callback_created)(t_config*), void (*callback_updated)(t_config*), char** config_keys, int config_size, t_log* logger);
void update_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger);
void pre_check_config(config_args_t* args);
void check_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger);
bool validate_config(t_config* config, char** config_keys, int config_size, t_log* logger);
bool validate_config_properties(t_config* config, char** config_keys, int config_size, t_log* logger);
config_args_t* build_config_args(char** config_keys, int config_size, char* file, t_log* logger, void (*updated)(t_config*));
int init_int_config_value(char* key, t_config* config, t_log* logger);
int update_int_config_value(int current_value, char* key, t_config* config, t_log* logger);
char* init_str_config_value(char* key, t_config* config, t_log* logger);
char* update_str_config_value(char* destination, char* key, t_config* config, t_log* logger);
t_list* init_str_array_config_value(char* key, t_config* config, t_log* logger);
t_list* init_int_array_config_value(char* key, t_config* config, t_log* logger);

#endif /* GENERIC_CONFIG_H_ */
