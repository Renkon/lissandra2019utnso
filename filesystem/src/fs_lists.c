#include "fs_lists.h"
//Creo esta variable global para poder usar el find de las commons
char* possible_table_name;

void assing_table_name_to_search(char* new_name) {
	possible_table_name = new_name;
}

void free_table_name_to_search() {
	free(possible_table_name);
}

bool table_exist(table_t* table) {
	return possible_table_name == table->name;
}

bool table_not_exist_in_list(t_list* list, char* table_name) {
	for (int i = 0; i < list->elements_count; i++) {
		table_t* table = list_get(list, i);
		if (strcmp(table->name,table_name)==0) {

			return false;

		}
	}
	return true;
}


table_t* find_table_in_list(t_list* list, char* table_name) {

	for (int i = 0; i < list->elements_count; i++) {
		table_t* table = list_get(list, i);
		if (strcmp(table->name,table_name)==0) {
			//Si encontre la tabla la devuelvo
			return table;

		}
	}
	table_t* table_default = malloc(sizeof(table_t));
	table_default->name = "Table not found";
	//Si no, devuelvo una default, aunque la idea es siempre usar esta funcion sabiendo que voy a encontrar la tabla
	return table_default;
}
