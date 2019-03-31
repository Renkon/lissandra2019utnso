#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "commons/log.h"

typedef struct {
	int port;
	char* mount_point;
	int delay;
	int max_value_size;
	int dump_time;
} fsconfig_t;

fsconfig_t g_config;
t_log* g_logger;


#endif /* FILESYSTEM_H_ */
