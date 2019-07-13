#include "lissandra.h"

// Las keys de configuracion
char* g_config_keys[] = { "PUERTO_ESCUCHA", "PUNTO_MONTAJE", "RETARDO", "TAMAÑO_VALUE", "TIEMPO_DUMP" };
int g_config_keys_size = 5;

int main(void) {
	if (!init_logger("filesystem.log", "Filesystem", true, LOG_LEVEL_TRACE))return 1;
	mem_table = list_create();
	get_tmpc_name = "A1.tmpc";
	init_config(FSCFG, initialize_fs_config, update_fs_config, g_config_keys, g_config_keys_size);
	fs_metadata = read_fs_metadata();
	bitmap_semaphore = malloc(sizeof(sem_t));
	sem_init(bitmap_semaphore, 0,1);
	sem_init(&thread_semaphore, 0,0);
	initialize_dump();
	table_state_list= create_table_state_list();
	iniitalize_compaction_in_all_tables();
	init_server_callbacks();
	init_server(g_config.port, FILESYSTEM);
	setup_response_id_generator();
	init_console("Papito codeo en Assembler - Filesystem v1.0", "filesystem>", FILESYSTEM, get_callbacks());
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return get_input_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, NULL, NULL, NULL, NULL);
}

void init_server_callbacks() {
	g_server_callbacks[SELECT_IN] = process_select;
	g_server_callbacks[INSERT_IN] = process_insert;
	g_server_callbacks[CREATE_IN] = process_create;
	g_server_callbacks[DESCRIBE_IN] = process_describe;
	g_server_callbacks[DROP_IN] = process_drop;
	g_server_callbacks[VALUE_IN] = process_value;
	g_server_callbacks[MULTIINSERT_IN] = process_multiinsert;
}
