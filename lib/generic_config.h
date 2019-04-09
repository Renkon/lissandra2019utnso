#ifndef GENERIC_CONFIG_H_
#define GENERIC_CONFIG_H_
#define EVENT_SIZE    ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

#include "commons/config.h"
#include "commons/collections/list.h"
#include "generic_logger.h"
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
} config_args_t;

bool init_config(char* config_file, void (*callback_created)(t_config*), void (*callback_updated)(t_config*), char** config_keys, int config_size);
void update_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size);
void pre_check_config(config_args_t* args);
void check_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size);
bool validate_config(t_config* config, char** config_keys, int config_size);
bool validate_config_properties(t_config* config, char** config_keys, int config_size);
config_args_t* build_config_args(char** config_keys, int config_size, char* file, void (*updated)(t_config*));
int init_int_config_value(char* key, t_config* config);
int update_int_config_value(int current_value, char* key, t_config* config);
char* init_str_config_value(char* key, t_config* config);
char* update_str_config_value(char* destination, char* key, t_config* config);
t_list* init_str_array_config_value(char* key, t_config* config);
t_list* init_int_array_config_value(char* key, t_config* config);

#endif /* GENERIC_CONFIG_H_ */
