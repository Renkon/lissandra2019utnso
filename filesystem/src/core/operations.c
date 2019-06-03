#include "operations.h"

void process_select(select_input_t* input) {
	log_i("fs select args: %s %u", input->table_name,(unsigned int) input->key);
	char* table_name_upper = to_uppercase(input->table_name);
	//Primero me fijo si existe la tabla
	if (exist_in_directory(input->table_name, get_table_directory())) {

		char* table_directory = create_new_directory(get_table_directory(),table_name_upper);
		record_t* key_found = search_key(table_directory, input->key);

		if (key_found->timestamp == -1) {
			//Si la key encotnrada me da una timstamp en -1 entonces significa que no la encontro
			log_w("La clave %d no existe en la tabla %s. Operacion SELECT cancelada",input->key, table_name_upper);

		} else {
			//SI la timstamp es distinta de -1 entonces si la encontre y la muestro!
			log_i("Clave %d encontrada en la tabla %s! su valor es: %s",input->key, table_name_upper, key_found->value);

		}
		free(table_directory);
		free(key_found);
	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion SELECT cancelada",table_name_upper);
	}

	free(table_name_upper);
}

void process_insert(insert_input_t* input) {
	log_i("fs insert args: %s %u \"%s\" %lld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	char* table_name_upper = to_uppercase(input->table_name);
	//Me fijo si existe la tabla
	if (exist_in_directory(input->table_name, get_table_directory())) {

		if (input->timestamp == -1) {
			input->timestamp =  get_timestamp();
		}

		record_t* record = create_record(input);

		if (table_not_exist_in_list(mem_table, table_name_upper)) {
			//Si no existe la tabla en la mem table la creo
			table_t* new_table = create_table(table_name_upper);
			list_add(mem_table, new_table);
		}
		//Siempre busco la tabla que necesito y despues le inserto la key, por ahora sin orden despues quizas si
		table_t* table = find_table_in_list(mem_table, table_name_upper);
		list_add(table->records, record);
		//Se les hace free cuando limpie la memetable despues.
		log_i("Se inserto satisfactoriamente la clave %d con valor %s y timestamp %lld en la tabla %s ",input->key, input->value, input->timestamp, table_name_upper);

	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %lld  no existe. Operacion INSERT cancelada", table_name_upper);
	}

	free(table_name_upper);
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
			log_i("Se creo el directorio de la tabla %s ", table_name_upper);

			create_table_metadata(input->consistency, input->partitions,input->compaction_time, table_name_upper);
			log_i("Se creo la metadata de la tabla %s ", table_name_upper);

			create_partitions(input->partitions, table_name_upper, blocks);
			write_bitmap(bitmap, get_bitmap_directory());
			log_i("Se crearon %d particiones para la tabla %s ",input->partitions, table_name_upper);
			log_i("Tabla %s creada exitosamente! ", table_name_upper);
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
	//Si me mandan null muestro la metadata de todas las tablas
	if (input->table_name == NULL) {
		t_list* table_list =get_tables_list();

		for(int i=0; i<table_list->elements_count; i++){
			char* table_name =list_get(table_list,i);
			char* table_directory = create_new_directory(get_table_directory(),table_name);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
			"\n Una consistencia del tipo %s \n Y %d particion/es.   ", table_name,table_metadata->compaction_time,consistency_name,table_metadata->partitions);
			free(table_directory);
			free(table_metadata);

		}


	} else {
		//Si no, me fijo si la tabla que me mando existe
		char* table_name_upper = to_uppercase(input->table_name);
		if (exist_in_directory(input->table_name, get_table_directory())) {
			//Si existe muestro su metadata
			char* table_directory = create_new_directory(get_table_directory(),table_name_upper);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
					"\n Una consistencia del tipo %s \n Y %d particion/es.   ", table_name_upper,table_metadata->compaction_time,consistency_name,table_metadata->partitions);
			free(table_directory);
			free(table_metadata);
		} else {
			//Si no existe la tabla  se termina la operacion
			log_w("La tabla %s no existe. Operacion DESCRIBE cancelada",table_name_upper);
		}
		free(table_name_upper);
	}

}

void process_drop(drop_input_t* input) {
	log_i("fs drop args: %s", input->table_name);
}
