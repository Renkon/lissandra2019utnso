
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "generic_config.h"

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
t_log* g_logger;

#endif /* MEMORIA_H_ */
