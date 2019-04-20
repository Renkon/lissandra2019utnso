
#ifndef KERNEL_H_
#define KERNEL_H_
#define KNCFG "kernel.cfg"

#include "config.h"
#include "generic_console.h"
#include "generic_config.h"
#include "generic_logger.h"
#include "core/operations.h"
#include "core/planifier.h"

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

callbacks_t* get_callbacks();

#endif /* KERNEL_H_ */
