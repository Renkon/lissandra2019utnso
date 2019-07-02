
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "memory_utils.h"
#include "memory_types.h"

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

typedef struct {
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
} memconfig_t;


memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;

void init_server_callbacks();
void create_dummy();

//GLOBAL STATMENTS
t_list* g_segment_list;
char** main_memory;
int total_page_count;
int total_page_size;

callbacks_t* get_callbacks();
void init_main_memory();

#endif /* MEMORIA_H_ */
