#include "table_utils.h"


table_t* create_table(char* name){
	table_t* new_table = malloc(sizeof(table_t));
	new_table->name = malloc(strlen(name)+1);
	strcpy(new_table->name, name);
	new_table->tkvs = list_create();
	return new_table;

}
