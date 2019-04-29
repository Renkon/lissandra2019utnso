#include "operations.h"

void process_select(select_input_t* input) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);
	log_i("la cantidad de elementos que tengo es: %d",g_segment_list->elements_count);
	/*char* demo_str = string_duplicate("soy una memoria");
	page_t* page_found;
	char* segment_found;

	segment_found=malloc(sizeof(input->table_name));
	memcpy(segment_found,segment_exists(input->table_name),strlen(segment_exists(input->table_name))+1);

	if(segment_found != NULL){
		find_page(segment_found,input->key);
		if(page_found==NULL){
			//se lo pido al filesystem que se encargue y me devuelva lo que deba
			//do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, demo_str, 16, select_callback);
		}else{
			//logica de retorno por consola
		};
	};*/
}

void process_insert(insert_input_t* input) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);

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
	log_i("Recibi respuesta del servidor: %s", (char*) response);
}

// lo siguiente no se si seria necesario ya que el create solo pasa la operacion al FS
/*void create_callback(void) {
	log_i("Se envio la operacion al FileSysyem");
}*/

/*void describe_callback(void){
	// faltaria completar el tipo de respuesta
}*/

char* segment_exists(char* segment_name){
	//amplio logica despues
	return 0;
}

void find_page(char* segment,uint16_t key ){
	//amplio logica despues, esta funcion devuelve un char*
}
