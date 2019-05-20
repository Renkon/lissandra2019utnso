#include "operations.h"

void process_select(select_input_t* input) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);
	log_i("la cantidad de elementos que tengo es: %d",g_segment_list->elements_count);

	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, 16, select_callback);
	//char* demo_str = string_duplicate("soy una memoria");
	page_t* found_page;
	segment_t* found_segment

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){
		found_page = get_page_by_key(found_segment,input->key);
		if(found_page != NULL){
			//CALLBACK RETORNO CON LO QUE POSEO ACTUALMENTE
		}else{
			//CALLBACK CON LOGICA PARA PREGUNTARSELO AL FILESYSTEM Y DESPUES PASARLO AL KURRLUL
		};
	};
}

void process_insert(insert_input_t* input) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	segment_t* found_segment;
	page_t* found_page;
	record_t* modified_record;

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){
		found_page = get_page_by_key(found_segment,input->key);
		if(found_page != NULL){
			modified_record = found_page->record;
			modified_record->timestamp = get_timestamp();
			modified_record->value = input->value;
		}else{
			//pido pagina libre...que?
			modified_record = create_record(input->timestamp,input->key, input->value);
			found_page = create_page(list_size(found_segment->page),modified_record,true);
			list_add(found_segment,found_page);
			//falta expandir la logica de que pasa si no tengo espacio.
		}
	}else{
		found_segment = create_segment(input->table_name);
		modified_record = create_record(input->timestamp,input->key,input->value);
		found_page = create_page(list_size(found_segment->page),modified_record,true);
		list_add(found_segment->page,found_page);
		list_add(g_segment_list,found_segment);
 	};

}

void process_create(create_input_t* input) {
	log_i("mm create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
		// solo se envia al FileSystem la operacion para crear la tabla
		//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, demo_str, 16, create_callback);
}

void process_describe(describe_input_t* input) {
	log_i("mm describe args: %s", input->table_name);
	// se envia la operacion al filesystem,deberia retornar lo que el kernel necesite para la operacion
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DESCRIBE_IN, demo_str, 16, describe_callback);

}

void process_drop(drop_input_t* input) {
	log_i("mm drop args: %s", input->table_name);
	//verificar si existe el segmento de tabla en M.Principal
	//libero el espacio
	//informo al FS
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DROP_IN, demo_str, 16, drop_callback);
}

void process_journal() {
	log_i("mm journal args none");
}

void select_callback(void* response) {
	//creo un socket que invoque una funcion  y dicha funcion info con ese mismo socket...que es un socket?
}


void create_callback(void* response) {
	//lo mismo que el select callback, la funcion con el socket seguro tendre que hacerla polimorfica o weas
}

void describe_callback(void* response){
	//se lo pido al FS con la funcion para devolver parametros
}

segment_t* get_segment_by_name(t_list* list, char* table_name){
	 	int i = 0;
	 	segment_t* segment_found;

	 	for(;i < list_size(list);i++){
	 		segment_found = list_get(list,i);
	 		if(strcmp(segment_found->name,table_name) == 0){
	 			break;
	 		}
	 	}
	 	return i<list_size(list)?segment_found:NULL;
}

page_t* get_page_by_key(segment_t* segment, int key){
	t_list* list_page = segment->page;
	page_t* page_found;
	int i = 0;

	for(;i < list_size(list_page);i++){
		 		page_found = list_get(list_page,i);
		 		if(page_found->record->key == key){
		 			break;
		 		}
		 	}
		 	return i<list_size(list_page)?page_found:NULL;
}
