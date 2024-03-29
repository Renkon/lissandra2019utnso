
#ifndef KERNEL_H_
#define KERNEL_H_

#define KNCFG "kernel.cfg"

#include "config.h"
#include "generic_console.h"
#include "generic_config.h"
#include "generic_logger.h"
#include "core/operations.h"
#include "core/scheduler/scheduler.h"
#include "core/statistics/metrics.h"
#include "core/gossiping/gossiping.h"
#include "core/memories/memories.h"

typedef struct {
	char* memory_ip;
	int memory_port;
	int quantum;
	int multiprocessing;
	int metadata_refresh;
	int execution_delay;
	int lts_delay;
	int metrics_display;
	int gossip_refresh;
} knconfig_t;

knconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;

callbacks_t* get_callbacks();

#endif /* KERNEL_H_ */
