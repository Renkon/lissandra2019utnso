#include "operations.h"

void process_select(select_input_t* input) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);
	log_i("la cantidad de segmentos que tengo es: %d",g_segment_list->elements_count);

	page_t* found_page;
	segment_t* found_segment;

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){
		found_page = get_page_by_key(found_segment,input->key);
		if(found_page != NULL){
			//CALLBACK RETORNO CON LO QUE POSEO ACTUALMENTE
			log_i("Clave %u encontrada en la tabla %s ! Su valor es: %s ",(unsigned int)found_page->record->key, found_segment->name, found_page->record->value);
		}else{
			//Lo pido al FS

		};
	};
}

void process_insert(insert_input_t* input) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	segment_t* found_segment;
	page_t* found_page;
	record_t* modified_record;
	page_t* index_list;

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){

		index_list = list_map(found_segment->page,page_get_index);
		found_page = get_page_by_key(index_list,found_segment,input->key,main_memory);

		if(found_page != NULL){
			//TODO Tode este wee
			modify_memory_by_index(found_page->index,input->value,10000);
			found_page->modified = true;

			log_i("Se modifico el registro con key %u con el valor: %s ",found_page->index, modified_record->value);

		}else{
			//pido pagina libre...que?

			modified_record = create_record(input->timestamp,input->key, input->value);
			found_page = create_page(list_size(found_segment->page),modified_record,true);
			list_add(found_segment->page,found_page);

			//falta expandir la logica de que pasa si no tengo espacio.
			log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %u en la tabla %s", (unsigned int)modified_record->key, modified_record->value , (unsigned int)modified_record->timestamp, found_segment->name);
		}
	}else{
		found_segment = create_segment(input->table_name);
		modified_record = create_record(input->timestamp,input->key,input->value);
		found_page = create_page(list_size(found_segment->page),modified_record,true);
		list_add(found_segment->page,found_page);
		list_add(g_segment_list,found_segment);

		log_i("Se inserto satisfactoriamente la clave %u con valor %s y timestamp %u en la tabla %s", (unsigned int)modified_record->key, modified_record->value , (unsigned int)modified_record->timestamp, found_segment->name);
 	};

}

void process_create(create_input_t* input) {
	log_i("mm create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
		// solo se envia al FileSystem la operacion para crear la tabla
		//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, CREATE_IN, input, sizeof(input), create_callback);
}

void process_describe(describe_input_t* input) {
	log_i("mm describe args: %s", input->table_name);
	// se envia la operacion al filesystem,deberia retornar lo que el kernel necesite para la operacion
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DESCRIBE_IN, input->table_name,strlen(input->table_name), describe_callback);

}

void process_drop(drop_input_t* input) {
	log_i("mm drop args: %s", input->table_name);

	segment_t* found_segment;
	page_t* found_page;
	record_t* modified_record;

	found_segment = get_segment_by_name(g_segment_list,input->table_name);

	if(found_segment != NULL){
		remove_segment(found_segment);

		log_i("Se borro satisfactoriamente la tabla %d", found_segment->name);
	}else{
		log_i("No se encontro la tabla en memoria, se procede a enviar la peticion al FileSystem");
	}

	//informo al FS
	//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, DROP_IN, input->table_name, strlen(input->name), drop_callback);
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

page_t* get_page_by_key(t_list* index_list, int key,char** main_memory){
	page_t* page_found;
	int index;

	int i = 0;
	for(;i < list_size(index_list);i++){
	 		index = list_get(index_list,i);
	 		if(main_memory_value(main_memory,index) == key){
	 			break;
	 		}
	 	}
	if(i<list_size(index_list)){
		page_found = get_page_by_index(page_found,index);
		return page_found;
	}else{
		return NULL;
	}
}

int page_get_index(page_t* page){
	return page->index;
}

page_t* get_page_by_index(t_list* page,int index){
	int i = 0;
	page_t* page_found;

	for(;i < list_size(page);i++){
		page_found = list_get(page,i);
		if(page_found->index == index){
			break;
	 		}
	 	}
	return i<list_size(page)?page_found:NULL;
}
