
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "generic_config.h"

typedef struct {
	int port;
	char* fs_ip;
	int fs_port;
	char** ip_seeds;
	int** ports_seeds;
	int memory_ret;
	int fs_ret;
	int memory_size;
	int journal_ret;
	int gossiping_ret;
	int memory_number;


} memconfig_t;

memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;
t_log* g_logger;


#endif /* MEMORIA_H_ */
