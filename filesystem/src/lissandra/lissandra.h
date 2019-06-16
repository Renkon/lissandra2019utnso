#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#define FSCFG "filesystem.cfg"


#include "../config.h"
#include "generic_console.h"
#include "generic_config.h"
#include "generic_server.h"
#include "../core/operations.h"


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
extern t_list* mem_table;
metadata_t* fs_metadata;

callbacks_t* get_callbacks();

#endif /* FILESYSTEM_H_ */
