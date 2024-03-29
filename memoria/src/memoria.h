
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "memory_utils.h"
#include "memory_types.h"
#include "shared_types/shared_types.h"

#include "generic_config.h"
#include "generic_console.h"
#include "generic_server.h"
#include "config.h"
#include "utils/operation_types.h"
#include "core/operations.h"
#include <stdbool.h>
#include <commons/bitarray.h>
#include "utils/dates.h"
#include <limits.h>
#include "utils/numbers.h"
#include "utils/response.h"
#include "gossiping/gossiping.h"

typedef struct {
	char* ip;
	int port;
	char* filesystem_ip;
	int filesystem_port;
	t_list* seed_ips;
	t_list* seed_ports;
	int memory_delay;
	int filesystem_delay;
	int memory_size;
	int journal_delay;
	int gossip_delay;
	int memory_number;
	int value_delay;
} memconfig_t;

memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;

void init_server_callbacks();
void init_global_segment_list();

//GLOBAL STATMENTS
t_list* g_segment_list;
char* g_main_memory;
int g_total_page_count;
int g_total_page_size;
int g_value_size;

callbacks_t* get_callbacks();

#endif /* MEMORIA_H_ */
