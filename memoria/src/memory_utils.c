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

	for (int i = 0; i < g_total_page_count; i++) {
		if (strcmp(g_main_memory+(i*g_total_page_size), "null") == 0) {
			str_key = string_itoa(key);
			str_tstamp = string_from_format("%lld", timestamp);
			str_tstamp = realloc(str_tstamp, strlen(str_tstamp) + strlen(str_key) + strlen(value) + 3);

			strcat(str_tstamp, ";");
			strcat(str_tstamp, str_key);
			strcat(str_tstamp, ";");
			strcat(str_tstamp, value);

			strcpy(g_main_memory+(i*g_total_page_size), str_tstamp);

			free(str_key);
			free(str_tstamp);
			return (i*g_total_page_size);
		}
	}

	return -1;
}

bool memory_full() {
	for (int i = 0; i < g_total_page_count; i++) {
		if (strcmp(g_main_memory+(i*g_total_page_size), "null") == 0) {
			return false;
		}
	}

	return true;
}


char* main_memory_values(int index,memory_var_t type){
	char** our_array = string_split(g_main_memory+index, ";");
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
	strcpy(g_main_memory+index, str_tstamp);

	free(str_key);
	free(str_tstamp);
}

bool order_by_timestamp(int first_i,int second_i){
	char* timestamp_1 = main_memory_values(first_i,TIMESTAMP);
	char* timestamp_2 = main_memory_values(second_i,TIMESTAMP);

	long long t1 = string_to_long_long(timestamp_1);
	long long t2 = string_to_long_long(timestamp_2);

	free(timestamp_1);
	free(timestamp_2);

	return t1<t2;

}

void eliminate_page_instance_by_index(int index, char* table_name){
	segment_t* found_segment = get_segment_by_index_global(index);

	strcpy(g_main_memory+index,"null");

	int position = get_position_by_index(found_segment->page,index);

	if(position != -1){
		list_remove_and_destroy_element(found_segment->page,position,(void*) destroy_page);

		if (list_size(found_segment->page) == 0 && !string_equals_ignore_case(table_name,found_segment->name)){
			remove_segment(found_segment);
		}

	}else{
		log_e("No se encontro la pagina, abortando");
	}

}

t_list* list_add_multi_lists(t_list* pages_indexes){
	t_list* new_list = list_create();
	char* timestamp;
	char* key;
	for(int i = 0 ; i < pages_indexes->elements_count ; i++){
		insert_input_t* insert = malloc(sizeof(insert_input_t));
		timestamp = main_memory_values(list_get(pages_indexes,i),TIMESTAMP);
		key = main_memory_values(list_get(pages_indexes,i),KEY);

		insert->timestamp = string_to_long_long(timestamp);
		insert->key = string_to_uint16(key);
		insert->value = main_memory_values(list_get(pages_indexes,i),VALUE);
		insert->table_name = strdup(get_table_name_by_index(list_get(pages_indexes,i)));


		list_add(new_list,insert);
		free(timestamp);
		free(key);
	}

	return new_list;
}

void journaling(response_t* response){
	int sem_val;
	sem_getvalue(&g_mem_op_semaphore, &sem_val);
	if (sem_val > 0)
		sem_wait(&g_mem_op_semaphore);

	t_list* journal = get_pages_by_modified(true);
	t_list* indexes = list_map(journal,(void*)page_get_index);

	t_list* journal_list = list_add_multi_lists(indexes);

	elements_network_t elem_info = elements_multiinsert_in_info(journal_list);
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, MULTIINSERT_IN, journal_list, elem_info.elements, elem_info.elements_size, journal_callback, true, cleanup_journal_input, response);

	list_destroy(journal);
	list_destroy(indexes);

}

void init_auto_journal() {
	pthread_t journal_thread;
	if (pthread_create(&journal_thread, NULL, (void*) journal_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de journaling");
	}
}

void journal_continuously() {
	pthread_detach(pthread_self());

	while (true) {
		usleep(g_config.journal_delay * 1000);
		journaling(NULL);
	}
}

page_t* replace_algorithm(response_t* response,long long timestamp,int key, char* value, journal_invocation_t invocation, char* table_name){
	int index;
	page_t* found_page;
	int replace;
	t_list* not_modified_pages = get_pages_by_modified(false);

	if(!list_is_empty(not_modified_pages)){

		t_list* indexes = list_map(not_modified_pages,(void*) page_get_index);

		t_list* indexes_sorted = list_sorted(indexes, order_by_timestamp);

		replace = list_get(indexes_sorted,0);

		eliminate_page_instance_by_index(replace, table_name);

		index = memory_insert(timestamp, key, value);

		if (invocation == J_SELECT){
			found_page = create_page(index, false);
		} else if (invocation == J_INSERT){
			found_page = create_page(index, true);
		}

		list_destroy(not_modified_pages);
		list_destroy(indexes);
		list_destroy(indexes_sorted);
		sem_post_neg(&g_mem_op_semaphore);
		return found_page;

	}else{
		list_destroy(not_modified_pages);
		log_i("Memoria llena, iniciando journaling...");
		journal_register_t* reg = malloc(sizeof(journal_register_t));
		reg->key = key;
		reg->timestamp = timestamp;
		reg->table_name = strdup(table_name);
		reg->value = strdup(value);

		if (invocation == J_INSERT)
			reg->modified = true;
		else if (invocation == J_SELECT)
			reg->modified = false;

		if (response == NULL) {
			response = malloc(sizeof(response_t));
			response->id = -1337;
		}

		response->result = reg;

		journaling(response);

		return NULL;
	}
}

void init_value_checker() {
	pthread_t value_thread;

	if (pthread_create(&value_thread, NULL, (void*) check_value, NULL)) {
		log_e("No se pudo inicializar el hilo de gossiping");
	}
}

void check_value() {
	pthread_detach(pthread_self());
	while (true) {
		get_value_from_filesystem();
		usleep(g_config.value_delay * 1000);
	}
}

void get_value_from_filesystem() {
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, VALUE_IN, NULL, 0, NULL, get_value_callback, true, NULL, NULL);
}

void get_value_callback(void* result, response_t* response) {
	if (result == NULL) {
		log_w("El filesystem no respondio la solicitud del TAMAÑO_VALUE");
		if (g_value_size == -1) {
			log_e("No se pudo obtener el TAMAÑO_VALUE del filesystem, me cerrare.");
			exit(1);
		}
	} else {
		int* value = (int*) result;
		log_t("Request del VALUE al fs devolvio: %i", *value);

		if (*value != g_value_size) {
			bool memory_initialized = g_value_size != -1;

			if ((*value + 21) > g_config.memory_size) {
				log_e("No entra ni un registro en la memoria! Value muy bajo");
				exit(1);
			}

			log_t("Se cambio el tamaño maximo de los registros");
			g_value_size = *value;
			g_total_page_size = digits_in_a_number(USHRT_MAX) + digits_in_a_number(get_timestamp()) + g_value_size + 3;
			g_total_page_count = g_config.memory_size/g_total_page_size;

			if (memory_initialized)
				journaling(response);
			else
				init_main_memory();
		}
	}
}

void init_main_memory(){
	g_main_memory = (char*) malloc(g_config.memory_size);
	for(int i = 0; i < g_total_page_count; i++){
		strcpy(g_main_memory+(i*g_total_page_size),"null");
	}
}

void sem_post_neg(sem_t* sem) {
	int value;
	sem_getvalue(sem, &value);
	if (value <= 0)
		sem_post(sem);
}
