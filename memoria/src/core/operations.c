#include "operations.h"

void process_select(select_input_t* input, response_t* response) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);
	usleep(g_config.memory_delay * 1000);

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
				free(upper_table_name);
			} else {
				log_w("No se encontro la pagina en MEMORIA. Solicitando al FILESYSTEM");

				elements_network_t elem_info = elements_select_in_info(input);
				select_input_t* select_input = malloc(sizeof(select_input_t));
				select_input->table_name = upper_table_name;

				select_input->key = input->key;

				usleep(g_config.filesystem_delay * 1000);
				do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, select_input, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);

				};
			list_destroy(index_list);
		}else{
			log_w("No se encontro el segmento en MEMORIA. Solicitando al FILESYSTEM");

			elements_network_t elem_info = elements_select_in_info(input);
			select_input_t* select_input = malloc(sizeof(select_input_t));
			select_input->table_name = upper_table_name;

			select_input->key = input->key;

			usleep(g_config.filesystem_delay * 1000);
			do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, select_input, elem_info.elements, elem_info.elements_size, select_callback, true, cleanup_select_input, response);
		}

}

void process_insert(insert_input_t* input, response_t* response) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	usleep(g_config.memory_delay * 1000);

	segment_t* found_segment;
	page_t* found_page;
	t_list* index_list;
	int index;
	input->timestamp = get_timestamp();

	char* upper_table_name = malloc(strlen(input->table_name)+1);
	strcpy(upper_table_name,input->table_name);
	string_to_upper(upper_table_name);

	if(strlen(input->value) <= g_value_size){

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
					found_page = create_page(index, false);
					list_add(found_segment->page, found_page);

					log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s", input->key, input->value, input->timestamp, found_segment->name);
				} else { //Ya no hay paginas disponibles, uso el algoritmo de reemplazo

					found_page = replace_algorithm(input->timestamp, input->key, input->value);
					list_add(found_segment->page, found_page);

					log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s despues de usar el algoritmo de reemplazo.", input->key, input->value, input->timestamp, found_segment->name);
				}
			}

			list_destroy(index_list);
		} else { //No existe la tabla, la creo e inserto el valor
			found_segment = create_segment(upper_table_name);
			if (!memory_full()){
				index = memory_insert(input->timestamp, input->key, input->value);
				found_page = create_page(index, false);
			} else {
				found_page = replace_algorithm(input->timestamp, input->key, input->value);
			}
			list_add(found_segment->page, found_page);
			list_add(g_segment_list, found_segment);
			log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s recien creada", input->key, input->value, input->timestamp, found_segment->name);
		};
	}else{
		log_w("El tamaño del value es mayor al maximo disponible, cancelando operacion.");
	}

	free(upper_table_name);
}

void process_create(create_input_t* input, response_t* response) {
	log_i("mm create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
	usleep(g_config.memory_delay * 1000);

	elements_network_t elem_info = elements_create_in_info(input);
	create_input_t* create_input = malloc(sizeof(create_input_t));
	create_input->table_name = strdup(input->table_name);
	create_input->compaction_time = input->compaction_time;
	create_input->consistency = input->consistency;
	create_input->partitions = input->partitions;

	usleep(g_config.filesystem_delay * 1000);
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, create_input, elem_info.elements, elem_info.elements_size,
			create_callback, true, cleanup_create_input, response);
}

void process_describe(describe_input_t* input, response_t* response) {
	log_i("mm describe args: %s", input->table_name);
	usleep(g_config.memory_delay * 1000);

	elements_network_t elem_info = elements_describe_in_info(input);
	describe_input_t* describe_input = malloc(sizeof(describe_input_t));
	if (input->table_name != NULL)
		describe_input->table_name = strdup(input->table_name);
	else
		describe_input->table_name = NULL;

	usleep(g_config.filesystem_delay * 1000);
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DESCRIBE_IN, describe_input, elem_info.elements, elem_info.elements_size,
			describe_callback, true, cleanup_describe_input, response);
}

void process_drop(drop_input_t* input, response_t* response) {
	log_i("mm drop args: %s", input->table_name);
	usleep(g_config.memory_delay * 1000);

	char* upper_table_name = malloc(strlen(input->table_name)+1);
	strcpy(upper_table_name,input->table_name);
	string_to_upper(upper_table_name);
	segment_t* found_segment;

	found_segment = get_segment_by_name(g_segment_list,upper_table_name);

	if(found_segment != NULL){
		remove_segment(found_segment);

		log_i("Se borro satisfactoriamente la tabla %s. Enviamos peticion al Filesystem!", upper_table_name);
	}else{
		log_w("No se encontro la tabla en memoria, se procede a enviar la peticion al FileSystem");
	}

	elements_network_t elem_info = elements_drop_in_info(input);
	drop_input_t* drop_input = malloc(sizeof(drop_input_t));

	drop_input->table_name = upper_table_name;

	usleep(g_config.filesystem_delay * 1000);
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DROP_IN, drop_input, elem_info.elements, elem_info.elements_size, drop_callback, true, cleanup_drop_input, response);

}

void process_journal(response_t* response) {
	log_i("mm journal args none");
	usleep(g_config.memory_delay * 1000);

	journaling();

}

void select_callback(void* result, response_t* response) {
	// Falla conexion?
	record_t* alpha_record;
	record_t* record = (record_t*) result;

	if (result == NULL) {
		log_e("No se pudo seleccionar un valor del filesystem");
	} else {
		switch (record->timestamp){
		case -1:
			log_e("No existe el registro solicitado. Cancelando operacion");
			break;
		case -2:
			log_e("No existe la tabla solicituda. Cancelando operacion");
			break;
		default:

			alpha_record= malloc(sizeof(record_t));
			alpha_record->table_name = strdup(record->table_name);
			alpha_record->key = record->key;
			alpha_record->timestamp = record->timestamp;
			alpha_record->value = strdup(record->value);
			//log_i("Clave %s encontrada en la tabla %s ! Su valor es: %s ", alpha_record->key, alpha_record->table_name, alpha_record->value);
			log_i("Agregando en memoria el nuevo registro...");

			char* upper_table_name = strdup(alpha_record->table_name);
			string_to_upper(upper_table_name);
			page_t*	found_page;
			int index;
			t_list* index_list;
			segment_t* found_segment = get_segment_by_name(g_segment_list, upper_table_name);

			if (found_segment != NULL) {
				index_list = list_map(found_segment->page, (void*) page_get_index);
				found_page = get_page_by_key(found_segment, index_list, alpha_record->key);
					if (!memory_full()) {
						index = memory_insert(alpha_record->timestamp, alpha_record->key, alpha_record->value);
						found_page = create_page(index, false);
						list_add(found_segment->page, found_page);
						log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s", alpha_record->key, alpha_record->value, alpha_record->timestamp, found_segment->name);
					} else {
						found_page = replace_algorithm(alpha_record->timestamp, alpha_record->key, alpha_record->value);
						list_add(found_segment->page, found_page);
						log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s despues de usar el algoritmo de reemplazo.", alpha_record->key, alpha_record->value, alpha_record->timestamp, found_segment->name);
						}
				list_destroy(index_list);
			}else {
				found_segment = create_segment(upper_table_name);
				index = memory_insert(alpha_record->timestamp, alpha_record->key, alpha_record->value);
				found_page = create_page(index, true);
				list_add(found_segment->page, found_page);
				list_add(g_segment_list, found_segment);

				log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %lld en la tabla %s recien creada", alpha_record->key, record->value, alpha_record->timestamp, found_segment->name);
			}
			free(upper_table_name);
			free(alpha_record->table_name);
			free(alpha_record->value);
			free(alpha_record);
		}
	}

	if (response != NULL) { //me llega desde el kernel
		// Vamos a copiar el objeto record, asi se lo podemos devolver
		record_t* new_record = malloc(sizeof(record_t));
		if (result != NULL) {
			new_record->table_name = strdup(record->table_name);
			new_record->key = record->key;
			new_record->timestamp = record->timestamp;
			new_record->value = strdup(record->value);
		} else {
			new_record->table_name = strdup("ERROR");
			new_record->key = -3;
			new_record->timestamp = -3;
			new_record->value = strdup("-3");
		}
		set_response(response, new_record);
	}
}

void insert_callback(void* result, response_t* response) {
	int* output;

	if (result == NULL) {
		output = malloc(sizeof(int));
		*output = -3;
	} else {
		output = (int*) result;
	}

	if (*output == 0) {
		log_i("Se ha creado una tabla satisfactoriamente");
	} else if (*output == -1) {
		log_e("La tabla que se intenta crear ya existe");
	} else if (*output == -2) {
		log_e("No hay bloques en el FS para crear la tabla");
	} else if (*output == -3) {
		log_e("Hubo un error de red al ir a crear la tabla");
	}

	if (response != NULL) {
		int* new_result = malloc(sizeof(int));
		*new_result = *output;
		set_response(response, new_result);
	}

	if (result == NULL)
		free(output);
}

void create_callback(void* result, response_t* response) {
	int* output;

	if (result == NULL) {
		output = malloc(sizeof(int));
		*output = -3;
	} else {
		output = (int*) result;
	}

	if (*output == 0) {
		log_i("Se ha creado una tabla satisfactoriamente");
	} else if (*output == -1) {
		log_e("La tabla que se intenta crear ya existe");
	} else if (*output == -2) {
		log_e("No hay bloques en el FS para crear la tabla");
	} else if (*output == -3) {
		log_e("Hubo un error de red al ir a crear la tabla");
	}

	if (response != NULL) {
		int* new_result = malloc(sizeof(int));
		*new_result = *output;
		set_response(response, new_result);
	}

	if (result == NULL)
		free(output);
}

void describe_callback(void* result, response_t* response){
	t_list* results = (t_list*) result;
	table_metadata_t* table_metadata;
	char* consistency;

	if (result == NULL) {
		log_e("Hubo un error de red al hacer el DESCRIBE");
	} else {
		for (int i = 0; i < list_size(results); i++) {
			table_metadata = (table_metadata_t*) list_get(results, i);
			consistency = get_consistency_name(table_metadata->consistency);

			log_i("Metadata de tabla %s", table_metadata->table_name);
			log_i("Tiempo de compactacion: %ld", table_metadata->compaction_time);
			log_i("Consistencia: %s", consistency);
			log_i("Cantidad de particiones: %i", table_metadata->partitions);
		}
	}

	if (response != NULL) {
		t_list* new_results = list_create();
		table_metadata_t* new_table_metadata;

		if (result != NULL) {
			for (int i = 0; i < list_size(results); i++) {
				table_metadata = (table_metadata_t*) list_get(results, i);
				new_table_metadata = malloc(sizeof(table_metadata_t));
				new_table_metadata->table_name = strdup(table_metadata->table_name);
				new_table_metadata->compaction_time = table_metadata->compaction_time;
				new_table_metadata->consistency = table_metadata->consistency;
				new_table_metadata->partitions = table_metadata->partitions;

				list_add(new_results, new_table_metadata);
			}
		}
		set_response(response, new_results);
	}
}

void drop_callback(void* result, response_t* response){
	int* drop_status;

	if(result == NULL){
		drop_status = malloc(sizeof(int));
		*drop_status = -2;
	}else{
		drop_status = (int*) result;
	}

	if(*drop_status == 0){
		log_i("La tabla se borro satisfactoriamente.");
	}else if(*drop_status == -1){
		log_w("La tabla no existe. Operacion DROP cancelada");
	}else if(*drop_status == -2){
		log_e("Hubo un error de red al ir a droppear la tabla");
	}

	if (response != NULL) {
		int* new_status = malloc(sizeof(int));
		*new_status = *drop_status;
		set_response(response, new_status);
	}

	if(result == NULL){
		free(drop_status);
	}
}

void cleanup_select_input(void* input) {
	select_input_t* select_input = (select_input_t*) input;
	free(select_input->table_name);
	free(select_input);
}

void cleanup_insert_input(void* input) {
	insert_input_t* insert_input = (insert_input_t*) input;
	free(insert_input->table_name);
	free(insert_input->value);
	free(insert_input);
}

void cleanup_drop_input(void* input) {
	drop_input_t* drop_input = (drop_input_t*) input;
	free(drop_input->table_name);
	free(drop_input);
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
	if (result == NULL) {
		log_w("No me llego un value del FS. ¿Esta caido?");
		g_value_size = 4;
	} else {
		int* value = (int*) result;
		log_i("Me llego un value del FS. VALOR: %i", *value);
		g_value_size = *value;
	}
}

