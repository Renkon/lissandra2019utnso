#include "memoria.h"


// Las keys de configuracion de memoria
char* g_config_keys[] = { "PUERTO", "IP_FS", "PUERTO_FS", "IP_SEEDS", "PUERTO_SEEDS","RETARDO_MEM",
"RETARDO_FS","TAM_MEM","RETARDO_JOURNAL","RETARDO_GOSSIPING","MEMORY_NUMBER"};
int g_config_keys_size = 11;

//se supone que si los creo acá son variables globales para el proceso :B
t_list* segment_list = list_create();
page_t* page_dummy = createPage(100,1,"hola");
table_page_t* t_page_dummy = createTable(1,page_dummy,false);
segment_t* dummy = createSegment(t_page_dummy,"nell");
list_add(segment_list,dummy);

int main(void) {
	if (!init_logger("memoria.log", "Memoria", true, LOG_LEVEL_TRACE))
		return 1;

	init_config(MEMCFG, initialize_memory_config, update_memory_config, g_config_keys, g_config_keys_size);
	init_server(g_config.port, MEMORY);
	init_console("Papito codeo en Assembler - Memoria v1.0", "memory>", MEMORY, get_callbacks());
	printf(page_dummy->key);
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return build_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, process_journal, NULL, NULL, NULL);
}

segment_t *createSegment(table_page_t* table, char *table_name) {
  segment_t *segment = malloc(sizeof(segment_t));

  segment->table_page = *table;
  segment->name= malloc(strlen(table_name)+1);
  strcpy(segment->name,table_name);
  return segment;
}

table_page_t *createTable(int page_number,page_t* page, bool modified ) {
	table_page_t *table_page = malloc(sizeof(table_page_t));

	table_page->page_number = page_number;
	table_page->page = *page;
	table_page->modified=modified;

	return table_page;
}

page_t *createPage(long timestamp, int key, char* value) {
	page_t *page = malloc(sizeof(page_t));

	page->key=key;
	page->timestamp=timestamp;
	page->value= malloc(strlen(value)+1);
	strcpy(page->value,value);

	return page;
}
