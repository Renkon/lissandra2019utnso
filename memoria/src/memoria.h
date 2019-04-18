
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "generic_config.h"
#include "generic_console.h"
#include "generic_server.h"
#include "config.h"
#include "core/operations.h"
#include <stdbool.h>

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

typedef struct{
	long timestamp;
	int key;
	char* value;
} page;

typedef struct{
	int page_number;
	page page;
	bool modified;
} segment;


memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;

callbacks_t* get_callbacks();

#endif /* MEMORIA_H_ */
