#include "operations.h"

void process_select(select_input_t* input) {
	log_i("fs select args: %s %u", input->table_name,(unsigned int) input->key);
	//Primero me fijo si existe la tabla
	if(exist_in_directory(input->table_name,get_table_directory())){
		char* table_name_upper = to_uppercase(input->table_name);
		char *table_directory = create_new_directory(get_table_directory(), table_name_upper);
		 search_key (table_directory, input->key);
		 	 //FALTAN WEAS
	}else{
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion SELECT cancelada",input->table_name);}





}

void process_insert(insert_input_t* input) {
	log_i("fs insert args: %s %u \"%s\" %ld", input->table_name,(unsigned int) input->key, input->value, input->timestamp);
}

void process_create(create_input_t* input) {
	log_i("fs create args: %s %i %i %ld", input->table_name, input->consistency,input->partitions, input->compaction_time);
	char* table_name_upper = to_uppercase(input->table_name);
	t_bitarray* bitmap = read_bitmap(get_bitmap_directory());
	int blocks[(input->partitions)];

	//Quiero saber si hay tantos bloques libres como particiones asi que busco cuantos bloques libres hay
	int free_blocks_amount = assign_free_blocks(bitmap, blocks,input->partitions);

	if (free_blocks_amount == input->partitions) {
		//Si habia n bloques libres me toca saber si ya existe la tabla a crear o no
		if (create_table_folder(table_name_upper) == 0) {

			//SI no existe la tabla entonces procedo con el CREATE normlamente
			log_i("Se creo la tabla %s ", table_name_upper);

			create_table_metadata(input->consistency, input->partitions,input->compaction_time, table_name_upper);
			log_i("Se creo la metadata de la tabla %s ", table_name_upper);

			create_partitions(input->partitions, table_name_upper,blocks);
			write_bitmap(bitmap,get_bitmap_directory());
			log_i("Se crearon %d particiones para la tabla %s ",input->partitions, table_name_upper);

		} else {

			log_w("La tabla %s ya esta en el sistema. Operacion CREATE cancelada.",table_name_upper);

		}

	} else {
		//SI no habia tantos bloques libres como particiones quiero tener, entonces cancelo el CREATE
		log_w("No hay bloques suficientes como para crear la tabla con %d particiones. Operacion CREATE cancelada",input->partitions);

	}

	free(table_name_upper);
    free(bitmap);

}

void process_describe(describe_input_t* input) {
	log_i("fs describe args: %s", input->table_name);
}

void process_drop(drop_input_t* input) {
	log_i("fs drop args: %s", input->table_name);
}
