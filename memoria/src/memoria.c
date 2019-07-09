#include "memoria.h"


// Las keys de configuracion de memoria
char* g_config_keys[] = { "IP", "PUERTO", "IP_FS", "PUERTO_FS", "IP_SEEDS", "PUERTO_SEEDS","RETARDO_MEM",
		"RETARDO_FS", "TAM_MEM", "RETARDO_JOURNAL", "RETARDO_GOSSIPING", "MEMORY_NUMBER", "VALUE_DELAY"};
int g_config_keys_size = 13;

int main(void) {
	if (!init_logger("memoria.log", "Memoria", true, LOG_LEVEL_TRACE))
		return 1;

	init_config(MEMCFG, initialize_memory_config, update_memory_config, g_config_keys, g_config_keys_size);
	init_server_callbacks();
	init_server(g_config.port, MEMORY);
	init_value_checker();
	init_gossiping();
	setup_response_id_generator();
	init_global_segment_list();
	g_value_size = -1;
	init_console("Papito codeo en Assembler - Memoria v1.0", "memory>", MEMORY, get_callbacks());
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return get_input_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, process_journal, NULL, NULL, NULL);
}

void init_server_callbacks() {
	g_server_callbacks[SELECT_IN] = process_select;
	g_server_callbacks[INSERT_IN] = process_insert;
	g_server_callbacks[CREATE_IN] = process_create;
	g_server_callbacks[DESCRIBE_IN] = process_describe;
	g_server_callbacks[DROP_IN] = process_drop;
	g_server_callbacks[JOURNAL_IN] = process_journal;
	g_server_callbacks[GOSSIP_IN] = on_gossip;
}

void init_global_segment_list(){
	g_segment_list = list_create();

}

