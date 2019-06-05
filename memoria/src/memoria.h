
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
#include <commons/bitarray.h>

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
	int charsize;
} record_t;

typedef struct {
	int index;
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
page_t* create_page(int index, bool modified );
segment_t* create_segment(char* table_name);

//son para las operations, no me dejaba ponerlos en el operations.h
segment_t* get_segment_by_name(t_list* list, char* table_name);
page_t* get_page_by_key(segment_t* segment, int key);
page_t* get_page_by_key(t_list* index_list, int key,char** main_memory);
page_t* get_page_by_index(t_list* page,int index);
int memory_insert(long long timestamp, int key, char* value, char** main_mem);

//GLOBAL STATMENTS
t_list* g_segment_list;
char** main_memory;
int total_memory_size;
int total_page_size;

callbacks_t* get_callbacks();
void init_main_memory();

#endif /* MEMORIA_H_ */
