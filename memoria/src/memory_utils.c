/*
 * memory_utils.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */
#include "memory_utils.h"

int memory_insert(long long timestamp, int key, char* value){
	char* str_key;
	char* str_tstamp;

	for (int i = 0; i < total_page_count; i++) {
		if (strcmp(main_memory[i], "null") == 0) {
			str_key = string_itoa(key);
			str_tstamp = string_from_format("%lld", timestamp);
			str_tstamp = realloc(str_tstamp, strlen(str_tstamp) + strlen(str_key) + strlen(value) + 3);

			strcat(str_tstamp, ";");
			strcat(str_tstamp, str_key);
			strcat(str_tstamp, ";");
			strcat(str_tstamp, value);

			strcpy(main_memory[i], str_tstamp);

			free(str_key);
			free(str_tstamp);
			return i;
		}
	}

	return -1;
}

bool memory_full() {
	for (int i = 0; i < total_page_count; i++) {
		if (strcmp(main_memory[i], "null") == 0) {
			return i >= total_page_count;
		}
	}

	return true;
}


char* main_memory_values(int index,memory_var_t type){
	char** our_array = string_split(main_memory[index], ";");
	char* value = our_array[type];
	switch( type ){
	case 0:
		free(our_array[1]);
		free(our_array[2]);
		break;
	case 1:
		free(our_array[0]);
		free(our_array[2]);
		break;
	case 2:
		free(our_array[0]);
		free(our_array[1]);
		break;
	default:
		free(our_array[0]);
		free(our_array[1]);
		free(our_array[2]);
		break;
	}
		free(our_array);

	return value;
}

void modify_memory_by_index(int index, int key , char* value){
	char* str_key = string_itoa(key);
	char* str_tstamp = string_from_format("%lld", get_timestamp());

	str_tstamp = realloc(str_tstamp, strlen(str_tstamp) + strlen(str_key) + strlen(value) + 3);

	strcat(str_tstamp, ";");
	strcat(str_tstamp, str_key);
	strcat(str_tstamp, ";");
	strcat(str_tstamp, value);
	strcpy(main_memory[index], str_tstamp);
}

void journaling(){
	t_list* journal = list_map(g_segment_list,memory_is_modified());
	t_list* indexes = list_map(journal,memory_is_modified_());
	//journal_type* journal = conformado por un registro_t y un nombre de la tabla
	int position;

	for(int i = 0; i < list_size(journal); i++){
		//journal = list_get(journal,0);
		//TODO enviar al FILESYSTEM la peticion de lo que esta asignado en la variable journal.
	}

	list_destroy(journal);

	for(int j = 0; j < list_size(journal); j++){
		position = list_get(indexes,0);
		strcpy(main_memory[position],"null");
	}
}

void replace_algorithm(){
	t_list* indexes = list_map(g_segment_list,page_modified());
	indexes = list_sort(indexes,order_by_timestamp());
	int replace = get_list(indexes,0);
	page_t* page = get_page_by_index(replace);
	//TODO continua ejecucion normal con esta page xd
}
