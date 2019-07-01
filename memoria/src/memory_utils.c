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

_Bool memory_full() {
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

bool order_by_timestamp(int first_i,int second_i){
	char* timestamp_1 = main_memory_values(first_i,TIMESTAMP);
	char* timestamp_2 = main_memory_values(second_i,TIMESTAMP);

	long long t1 = string_to_long_long(timestamp_1);
	long long t2 = string_to_long_long(timestamp_2);

	return t1>t2;

}

void eliminate_page_instance_by_index(segment_t* segment, int index){
	page_t* page = get_page_by_index(segment, index);
	int index_in_memory = page->index;

	strcpy(main_memory[index_in_memory],"null");

	int position = get_position_by_index(segment->page,index);

	if(position != -1){
		list_remove_and_destroy_element(segment->page,position,(void*) destroy_page);
	}else{
		log_w("NO se encontro la pagina, abortando");//nunca deberia pasar esto xd
	}
}

t_list* list_add_multi_lists(t_list* pages_indexes){
	t_list* new_list = list_create();
	insert_input_t* insert = malloc(sizeof(insert_input_t));

	for(int i = 0;i < list_size(pages_indexes); i++){
		insert->timestamp = string_to_long_long(main_memory_values(list_get(pages_indexes,i),TIMESTAMP));
		insert->key = string_to_uint16(main_memory_values(list_get(pages_indexes,i),KEY));
		insert->value = main_memory_values(list_get(pages_indexes,i),VALUE);
		insert->table_name = get_table_name_by_index(list_get(pages_indexes,i));

		list_add(new_list,insert);
	}

	return new_list;
}

void journaling(){
	t_list* journal = get_pages_by_modified(true);
	t_list* indexes = list_map(journal,(void*)page_get_index);

	t_list* journal_list = list_add_multi_lists(journal_list);

	int position;

	for(int i = 0; i < list_size(journal_list); i++){
		//insert_input_t* sending_journal = list_get(journal_list,i);
		//elements_network_t elem_info = elements_create_in_info(sending_journal);
		//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, sending_journal, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);

	}

	remove_pages_modified();

	for(int j = 0; j < list_size(journal); j++){
		position = list_get(indexes,j);
		strcpy(main_memory[position],"null");
	}

	list_destroy(journal);
	list_destroy(indexes);
}

page_t* replace_algorithm(segment_t* segment,long long timestamp,int key, char* value){
	t_list* not_modified_pages = get_pages_by_modified(false);
	int index;
	page_t* found_page;

	if(list_size(not_modified_pages) != 0){

		t_list* indexes = list_map(not_modified_pages,(void*) page_get_index);

		indexes = list_sorted(indexes,order_by_timestamp);

		int replace = list_get(indexes,0);

		eliminate_page_instance_by_index(segment,replace);

		index = memory_insert(timestamp, key, value);
		found_page = create_page(index, true);

		return found_page;

	}else{

		journaling();
		replace_algorithm(segment, timestamp, key, value);

	}
}
