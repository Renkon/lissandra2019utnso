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
		// solo se envia al FileSystem la operacion para crear la tabla
		//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, input, sizeof(input), create_callback);
}

void process_describe(describe_input_t* input, response_t* response) {
	log_i("mm describe args: %s", input->table_name);
	// se envia la operacion al filesystem,deberia retornar lo que el kernel necesite para la operacion
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DESCRIBE_IN, input->table_name,strlen(input->table_name), describe_callback);

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
		new_record->key = record->key;
		new_record->timestamp = record->timestamp;
		new_record->value = strdup(record->value);
		set_response(response, new_record);
	}
}


void create_callback(void* result, response_t* response) {
	//lo mismo que el select callback, la funcion con el socket seguro tendre que hacerla polimorfica o weas
}

void describe_callback(void* result, response_t* response){
	//se lo pido al FS con la funcion para devolver parametros
}

void cleanup_select_input(void* input) {
	select_input_t* select_input = (select_input_t*) input;
	free(select_input->table_name);
	free(select_input);
}

void get_value_from_filesystem() {
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, VALUE_IN, NULL, 0, NULL, get_value_callback, true, NULL, NULL);
}

void get_value_callback(void* result, response_t* response) {
	int* value = (int*) result;
	log_i("Me llego un value del FS. VALOR: %i", *value);
}

