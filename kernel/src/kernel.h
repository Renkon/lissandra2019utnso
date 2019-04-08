
#ifndef KERNEL_H_
#define KERNEL_H_
#define KNCFG "kernel.cfg"

#include "config.h"
#include "generic_console.h"
#include "generic_config.h"
#include "commons/config.h"
#include "commons/log.h"

typedef struct {
	char* memory_ip;
	int memory_port;
	int quantum;
	int multiprocessing;
	int metadata_refresh;
	int execution_delay;
} knconfig_t;

knconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;
t_log* g_logger;

#endif /* KERNEL_H_ */
