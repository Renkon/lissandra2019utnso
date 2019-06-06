#include "memoria.h"


// Las keys de configuracion de memoria
char* g_config_keys[] = { "PUERTO", "IP_FS", "PUERTO_FS", "IP_SEEDS", "PUERTO_SEEDS","RETARDO_MEM",
"RETARDO_FS","TAM_MEM","RETARDO_JOURNAL","RETARDO_GOSSIPING","MEMORY_NUMBER"};
int g_config_keys_size = 11;

int main(void) {
	if (!init_logger("memoria.log", "Memoria", true, LOG_LEVEL_TRACE))
		return 1;

	init_config(MEMCFG, initialize_memory_config, update_memory_config, g_config_keys, g_config_keys_size);

	total_page_size = sizeof(long long)+sizeof(int)+(4*sizeof(char))+(2*sizeof(char)); //TODO el size esta hardcodeado, lo pasa las configs de FS
	total_memory_size = g_config.memory_size/total_page_size;

	init_server(g_config.port, MEMORY);
	init_main_memory();
	create_dummy();
	init_console("Papito codeo en Assembler - Memoria v1.0", "memory>", MEMORY, get_callbacks());
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return build_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, process_journal, NULL, NULL, NULL);
}


page_t* create_page(int index, bool modified ) {
	page_t* page = malloc(sizeof(page_t));

	page->index = index;
	page->modified = modified;

	return page;
}

segment_t* create_segment(char* table_name) {
  segment_t* segment = malloc(sizeof(segment_t));

  segment->page = list_create();
  segment->name = malloc(strlen(table_name)+1);
  strcpy(segment->name,table_name);

  return segment;
}

void create_dummy(){
	int a;
	g_segment_list = list_create();
	segment_t* segment_dummy = create_segment("laposta");

	a = memory_insert(40000000,5,"hey");
	page_t* page_dummy = create_page(a,false);

	list_add(segment_dummy->page,page_dummy);
	list_add(g_segment_list,segment_dummy);
}

void init_main_memory(){
	main_memory = (char**) malloc(total_memory_size);
	for(int i=0;i<(total_memory_size);i++){
		main_memory[i] = (char*) malloc(total_page_size);
	}
}

