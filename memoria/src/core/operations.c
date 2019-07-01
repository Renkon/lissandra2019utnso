#include "operations.h"

void process_select(select_input_t* input, response_t* response) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);

//	page_t* found_page;
//	segment_t* found_segment;
//	char* return_timestamp;
//	char* return_key;
//	char* return_value;
//	int position;
//	t_list* index_list;
//
//	found_segment = get_segment_by_name(g_segment_list, input->table_name);
//
//	if (found_segment != NULL) {
//		index_list = list_map(found_segment->page, (void*) page_get_index);
//		found_page = get_page_by_key(found_segment, index_list, input->key);
//		if(found_page != NULL) {
//
//			position = found_page->index;
//			/*return_timestamp = main_memory_timestamp(position);
//			return_key = main_memory_key(position);
//			return_value = main_memory_value(position);*/
//
//			return_timestamp = main_memory_values(position,TIMESTAMP);
//			return_key = main_memory_values(position,KEY);
//			return_value = main_memory_values(position,VALUE);
//
//			log_i("Clave %s encontrada en la tabla %s ! Su valor es: %s ", return_key, found_segment->name, return_value);
//
//			free(return_timestamp);
//			free(return_key);
//			free(return_value);
//		} else {
//			/*//TODO Lo pido al FS, las variables no van a funcionar es hasta que tengamos la conexion con el FS
//
//			if(return_timestamp != -1){ //Si devuelve -1 significa que no lo encontro, tiramos un warning
//
//				if(!memory_full()){ //TODO este es mi algoritmo de reemplazo..deberia expandirlo más
//
//					position = memory_insert(return_timestamp,input->key,return_value);
//					found_page = create_page(position,false);
//					list_add(found_segment->page,found_page);
//
//				}
//			}else{
//				//TODO warning
//			};*/
//		}
//
//		list_destroy(index_list);
//	}

	elements_network_t elem_info = elements_select_in_info(input);
	select_input_t* select_input = malloc(sizeof(select_input_t));
	select_input->table_name = malloc(strlen(input->table_name) + 1);

	select_input->key = input->key;
	memcpy(select_input->table_name, input->table_name, strlen(input->table_name) + 1);
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, select_input, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);

	// LA LIMPIEZA DE LAS COSAS LA HACES EN EL CLEANUP CALLBACK, EN ESTE CASO, CLEANUP_SELECT_INPUT
}

void process_insert(insert_input_t* input, response_t* response) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	segment_t* found_segment;
	page_t* found_page;
	t_list* index_list;
	int index;
	input->timestamp = get_timestamp();

	if(strlen(input->value) <= 4){ //TODO Lo debemos traer de las configs del FS

		found_segment = get_segment_by_name(g_segment_list, input->table_name);

		if (found_segment != NULL) {
			index_list = list_map(found_segment->page, (void*) page_get_index);
			found_page = get_page_by_key(found_segment, index_list, input->key);

			if (found_page != NULL) {
				modify_memory_by_index(found_page->index, input->key, input->value);
				found_page->modified = true;

				log_i("Se modifico el registro con key %u con el valor: %s ",input->key, input->value);
			} else {
				if (!memory_full()) {
					index = memory_insert(input->timestamp, input->key, input->value);
					found_page = create_page(index, true);
					list_add(found_segment->page, found_page);

					log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s", input->key, input->value, input->timestamp, found_segment->name);
				} else {
					//TODO JOURNALING + inserto devuelta.
				}
			}

			list_destroy(index_list);
		} else {
			found_segment = create_segment(input->table_name);
			index = memory_insert(input->timestamp, input->key, input->value);
			found_page = create_page(index, true);
			list_add(found_segment->page, found_page);
			list_add(g_segment_list, found_segment);

			log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s recien creada", input->key, input->value, input->timestamp, found_segment->name);
		};
	}else{
		log_w("El tamaño del value es mayor al maximo disponible, cancelando operacion.");
	}
}

void process_create(create_input_t* input, response_t* response) {
	log_i("mm create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);

	elements_network_t elem_info = elements_create_in_info(input);
	create_input_t* create_input = malloc(sizeof(create_input_t));
	create_input->table_name = strdup(input->table_name);
	create_input->compaction_time = input->compaction_time;
	create_input->consistency = input->consistency;
	create_input->partitions = input->partitions;

	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, create_input, elem_info.elements, elem_info.elements_size,
			create_callback, true, cleanup_create_input, response);
}

void process_describe(describe_input_t* input, response_t* response) {
	log_i("mm describe args: %s", input->table_name);

	elements_network_t elem_info = elements_describe_in_info(input);
	describe_input_t* describe_input = malloc(sizeof(describe_input_t));
	if (input->table_name != NULL)
		describe_input->table_name = strdup(input->table_name);
	else
		describe_input->table_name = NULL;

	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DESCRIBE_IN, describe_input, elem_info.elements, elem_info.elements_size,
			describe_callback, true, cleanup_describe_input, response);
}

void process_drop(drop_input_t* input, response_t* response) {
	log_i("mm drop args: %s", input->table_name);

	segment_t* found_segment;
	page_t* found_page;
	record_t* modified_record;

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){
		//remove_segment(found_segment);

		log_i("Se borro satisfactoriamente la tabla %d", found_segment->name);
	}else{
		log_i("No se encontro la tabla en memoria, se procede a enviar la peticion al FileSystem");
	}

	//informo al FS
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DROP_IN, input->table_name, strlen(input->name), drop_callback);
}

void process_journal(response_t* response) {
	log_i("mm journal args none");
}

void select_callback(void* result, response_t* response) {
	record_t* record = (record_t*) result;

	if (response != NULL) {
		// Vamos a copiar el objeto record, asi se lo podemos devolver
		record_t* new_record = malloc(sizeof(record_t));
		new_record->table_name = strdup(record->table_name);
		new_record->key = record->key;
		new_record->timestamp = record->timestamp;
		new_record->value = strdup(record->value);
		set_response(response, new_record);
	}
}


void create_callback(void* result, response_t* response) {
	int* output = (int*) result;

	if (*output == 0) {
		log_i("Se ha creado una tabla satisfactoriamente");
	} else if (*output == -1) {
		log_e("La tabla que se intenta crear ya existe");
	} else if (*output == -2) {
		log_e("No hay bloques en el FS para crear la tabla");
	}

	if (response != NULL) {
		int* new_result = malloc(sizeof(int));
		*new_result = *output;
		set_response(response, new_result);
	}
}

void describe_callback(void* result, response_t* response){
	t_list* results = (t_list*) result;
	table_metadata_t* table_metadata;
	char* consistency;

	for (int i = 0; i < list_size(results); i++) {
		table_metadata = (table_metadata_t*) list_get(results, i);
		consistency = get_consistency_name(table_metadata->consistency);

		log_i("Metadata de tabla %s", table_metadata->table_name);
		log_i("Tiempo de compactacion: %ld", table_metadata->compaction_time);
		log_i("Consistencia: %s", consistency);
		log_i("Cantidad de particiones: %i", table_metadata->partitions);
	}

	if (response != NULL) {
		t_list* new_results = list_create();
		table_metadata_t* new_table_metadata;

		for (int i = 0; i < list_size(results); i++) {
			table_metadata = (table_metadata_t*) list_get(results, i);
			new_table_metadata = malloc(sizeof(table_metadata_t));
			new_table_metadata->table_name = strdup(table_metadata->table_name);
			new_table_metadata->compaction_time = table_metadata->compaction_time;
			new_table_metadata->consistency = table_metadata->consistency;
			new_table_metadata->partitions = table_metadata->partitions;

			list_add(new_results, new_table_metadata);
		}

		set_response(response, new_results);
	}
}

void cleanup_select_input(void* input) {
	select_input_t* select_input = (select_input_t*) input;
	free(select_input->table_name);
	free(select_input);
}

void cleanup_create_input(void* input) {
	create_input_t* create_input = (create_input_t*) input;
	free(create_input->table_name);
	free(create_input);
}

void cleanup_describe_input(void* input) {
	describe_input_t* describe_input = (describe_input_t*) input;
	if (describe_input->table_name != NULL)
		free(describe_input->table_name);
	free(describe_input);
}

void get_value_from_filesystem() {
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, VALUE_IN, NULL, 0, NULL, get_value_callback, true, NULL, NULL);
}

void get_value_callback(void* result, response_t* response) {
	int* value = (int*) result;
	log_i("Me llego un value del FS. VALOR: %i", *value);
}

