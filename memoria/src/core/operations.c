#include "operations.h"

void process_select(select_input_t* input, response_t* response) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);

	page_t* found_page;
	segment_t* found_segment;
	char* return_timestamp;
	char* return_key;
	char* return_value;
	int position;
	t_list* index_list;

	char* upper_table_name = malloc(strlen(input->table_name)+1);
	strcpy(upper_table_name,input->table_name);
	string_to_upper(upper_table_name);

		found_segment = get_segment_by_name(g_segment_list, upper_table_name);

		if (found_segment != NULL) {

			index_list = list_map(found_segment->page, (void*) page_get_index);
			found_page = get_page_by_key(found_segment, index_list, input->key);
			if(found_page != NULL) {

				position = found_page->index;
				return_timestamp = main_memory_values(position,TIMESTAMP);
				return_key = main_memory_values(position,KEY);
				return_value = main_memory_values(position,VALUE);

				log_i("Clave %s encontrada en la tabla %s ! Su valor es: %s ", return_key, found_segment->name, return_value);

				free(return_timestamp);
				free(return_key);
				free(return_value);
			} else {
				log_w("No se encontro la pagina en MEMORIA. Solicitando al FILESYSTEM");

				elements_network_t elem_info = elements_select_in_info(input);
				select_input_t* select_input = malloc(sizeof(select_input_t));
				select_input->table_name = upper_table_name;

				select_input->key = input->key;

				do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, select_input, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);

				};
			list_destroy(index_list);
		}else{
			log_w("No se encontro el segmento en MEMORIA. Solicitando al FILESYSTEM");

			elements_network_t elem_info = elements_select_in_info(input);
			select_input_t* select_input = malloc(sizeof(select_input_t));
			select_input->table_name = upper_table_name;

			select_input->key = input->key;

			do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, select_input, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);
		}

}

void process_insert(insert_input_t* input, response_t* response) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	segment_t* found_segment;
	page_t* found_page;
	t_list* index_list;
	int index;
	input->timestamp = get_timestamp();

	char* upper_table_name = malloc(strlen(input->table_name)+1);
	strcpy(upper_table_name,input->table_name);
	string_to_upper(upper_table_name);

	if(strlen(input->value) <= 4){ //TODO Lo debemos traer de las configs del FS

		found_segment = get_segment_by_name(g_segment_list, upper_table_name);

		if (found_segment != NULL) {//Existe la tabla, busco la pagina
			index_list = list_map(found_segment->page, (void*) page_get_index);
			found_page = get_page_by_key(found_segment, index_list, input->key);

			if (found_page != NULL) {//Existe la pagina, entonces modifico el record
				modify_memory_by_index(found_page->index, input->key, input->value);
				found_page->modified = true;

				log_i("Se modifico el registro con key %u con el valor: %s ",input->key, input->value);

			} else {//No existe la pagina

				if (!memory_full()) {//Aun hay paginas disponibles, inserto en el segmento una nueva pagina

					index = memory_insert(input->timestamp, input->key, input->value);
					found_page = create_page(index, true);
					list_add(found_segment->page, found_page);

					log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s", input->key, input->value, input->timestamp, found_segment->name);
				} else { //Ya no hay paginas disponibles, uso el algoritmo de reemplazo

					found_page = replace_algorithm(found_segment,input->timestamp, input->key, input->value);
					list_add(found_segment->page, found_page);

					log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s", input->key, input->value, input->timestamp, found_segment->name);
				}
			}

			list_destroy(index_list);
		} else { //No existe la tabla, la creo e inserto el valor
			found_segment = create_segment(upper_table_name);
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

	char* upper_table_name = malloc(strlen(input->table_name)+1);
	strcpy(upper_table_name,input->table_name);
	string_to_upper(upper_table_name);
	segment_t* found_segment;

	found_segment = get_segment_by_name(g_segment_list,upper_table_name);

	if(found_segment != NULL){
		remove_segment(found_segment);

		log_i("Se borro satisfactoriamente la tabla %s", upper_table_name);
	}else{
		log_w("No se encontro la tabla en memoria, se procede a enviar la peticion al FileSystem");
	}

	elements_network_t elem_info = elements_create_in_info(input);
	drop_input_t* drop_input = malloc(sizeof(drop_input_t));

	drop_input->table_name = upper_table_name;

	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DROP_IN, drop_input, elem_info.elements, elem_info.elements_size, drop_callback, true, cleanup_drop_input, response);


	free(found_segment);

}

void process_journal(response_t* response) {
	log_i("mm journal args none");
}

void select_callback(void* result, response_t* response) {
	record_t* record = (record_t*) result;

	switch (record->timestamp){
	case -1:
		log_w("No existe el registro solicitado.");
		break;
	case -2:
		log_w("No existe la tabla solicituda.");
		break;
	default:
		log_i("Oops");
		//TODO insert en la memoria principal
	}

	if (response != NULL) { //me llega desde el kernel
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

void drop_callback(void* result, response_t* response){
	int* drop_status = (int*) result;

	if(drop_status == 0){
		log_i("La tabla se borro satisfactoriamente.");
	}else{
		log_w("La tabla no existe. Operacion DROP cancelada");
	}

	if (response != NULL) {
		int* new_status = malloc(sizeof(int));
		*new_status = *drop_status;
		set_response(response, new_status);
	}
}

void cleanup_select_input(void* input) {
	select_input_t* select_input = (select_input_t*) input;
	free(select_input->table_name);
	free(select_input);
}


void cleanup_drop_input(void* input) {
	drop_input_t* drop_input = (drop_input_t*) input;
	free(drop_input->table_name);
	free(drop_input);
}

