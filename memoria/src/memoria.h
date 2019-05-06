
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
	long long timestamp;
	int key;
	char* value;
} record_t;

typedef struct {
	int page_number;
	record_t* record;
	bool modified;
} page_t;

typedef struct {
	char* name;
	t_list* page;
} segment_t;


memconfig_t g_config;
extern char* g_config_keys[];
extern int g_config_keys_size;
char* segment_exists(char* segment_name);
void find_page(char* segment,uint16_t key );
void create_dummy();

record_t* create_record(long timestamp, int key, char* value);
page_t* create_page(int page_number,record_t* record, bool modified );
segment_t* create_segment(char* table_name);

//son para las operations, no me dejaba ponerlos en el operations.h
bool is_our_segment(segment_t* segment, char* table_name);
bool is_our_page(page_t* page, int key);

//se supone que si los creo acá son variables globales para el proceso :B
t_list* g_segment_list;


callbacks_t* get_callbacks();

#endif /* MEMORIA_H_ */
