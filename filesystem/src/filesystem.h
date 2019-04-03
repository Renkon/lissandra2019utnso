#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#define FSCFG "filesystem.cfg"

#include "generic_config.h"

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
t_log* g_logger;

#endif /* FILESYSTEM_H_ */
