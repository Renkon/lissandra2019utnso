
#ifndef MEMORIA_H_
#define MEMORIA_H_
#define MEMCFG "memoria.cfg"

#include "generic_config.h"
#include "generic_console.h"
#include "generic_server.h"
#include "config.h"
#include "utils/operation_types.h"
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

typedef struct {
	long timestamp;
	int key;
	char* value;
} page_t;

typedef struct {
	int page_number;
	page_t page;
	bool modified;
} table_page_t;

typedef struct {
	char* name;
	table_page_t table_page;
} segment_t;


memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;
char* segment_exists(char* segment_name);
void find_page(char* segment,uint16_t key );
segment_t *createSegment(table_page_t* table, char *table_name);
table_page_t *createTable(int page_number,page_t* page, bool modified);
page_t *createPage(long timestamp, int key, char* value);

callbacks_t* get_callbacks();

#endif /* MEMORIA_H_ */
