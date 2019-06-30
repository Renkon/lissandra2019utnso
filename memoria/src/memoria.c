#include "memoria.h"


// Las keys de configuracion de memoria
char* g_config_keys[] = { "PUERTO", "IP_FS", "PUERTO_FS", "IP_SEEDS", "PUERTO_SEEDS","RETARDO_MEM",
"RETARDO_FS","TAM_MEM","RETARDO_JOURNAL","RETARDO_GOSSIPING","MEMORY_NUMBER"};
int g_config_keys_size = 11;

int main(void) {
	if (!init_logger("memoria.log", "Memoria", true, LOG_LEVEL_TRACE))
		return 1;

	init_config(MEMCFG, initialize_memory_config, update_memory_config, g_config_keys, g_config_keys_size);

	total_page_size = digits_in_a_number(USHRT_MAX) + digits_in_a_number(get_timestamp()) + (4*sizeof(char)) + (3*sizeof(char)); //TODO el size esta hardcodeado, lo pasa las configs de FS
	total_page_count = g_config.memory_size/total_page_size;
	init_server_callbacks();
	init_server(g_config.port, MEMORY);
	init_main_memory();
	setup_response_id_generator();
	create_dummy();
	get_value_from_filesystem();
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
}

void create_dummy(){ //TODO renombrar a init_global_segment o algo asi y borrar todo excepto el list_create de la segunda linea.
	int a;
	g_segment_list = list_create();
	segment_t* segment_dummy = create_segment("laposta");

	a = memory_insert(get_timestamp(),65000,"hey");
	page_t* page_dummy = create_page(a,false);

	list_add(segment_dummy->page,page_dummy);
	list_add(g_segment_list,segment_dummy);
}

void init_main_memory(){
	main_memory = (char**) malloc(total_page_count * sizeof(char*));
	for(int i = 0; i < total_page_count; i++){
		main_memory[i] = (char*) malloc(total_page_size);
		strcpy(main_memory[i], "null");
	}
}

