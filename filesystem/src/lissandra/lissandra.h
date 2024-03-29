#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#define FSCFG "filesystem.cfg"


#include "../config.h"
#include "generic_console.h"
#include "generic_config.h"
#include "generic_server.h"
#include "../core/operations.h"
#include "../filesystem/filesystem.h"
#include "utils/response.h"
#include "table_state_utils.h"

typedef struct {
	int port;
	char* mount_point;
	int delay;
	int max_value_size;
	int dump_time;
} fsconfig_t;

fsconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;
t_list* mem_table;
t_list* table_state_list;
metadata_t* fs_metadata;
sem_t* bitmap_semaphore;
sem_t thread_semaphore;
sem_t dump_semaphore;

callbacks_t* get_callbacks();
void init_server_callbacks();

#endif /* FILESYSTEM_H_ */
