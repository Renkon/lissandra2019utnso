#include "operations.h"

void process_select(select_input_t* input) {
	log_i("fs select args: %s %u", input->table_name,(unsigned int) input->key);
	char* table_name_upper = to_uppercase(input->table_name);
	char* initial_table_dir = get_table_directory();
	//Primero me fijo si existe la tabla
	if (exist_in_directory(input->table_name, initial_table_dir)) {

		char* table_directory = create_new_directory(initial_table_dir,table_name_upper);
		record_t* key_found = search_key(table_directory, input->key);

		if (key_found->timestamp == -1) {
			//Si la key encotnrada me da una timstamp en -1 entonces significa que no la encontro
			log_w("La clave %d no existe en la tabla %s. Operacion SELECT cancelada", input->key, table_name_upper);

		} else {
			//SI la timstamp es distinta de -1 entonces si la encontre y la muestro!
			log_i("Clave %d encontrada en la tabla %s! su valor es: %s", input->key, table_name_upper, key_found->value);
			free(key_found->value);
		}
		free(table_directory);
		free(key_found);
	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion SELECT cancelada",table_name_upper);
	}

	free(initial_table_dir);
	free(table_name_upper);
}

bool process_insert(insert_input_t* input) {
	log_i("fs insert args: %s %u \"%s\" %lld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	char* table_name_upper = to_uppercase(input->table_name);
	char* table_directory = get_table_directory();
	//Me fijo si existe la tabla
	if (exist_in_directory(input->table_name, table_directory)) {
		if (value_exceeds_maximun_size(input->value)) {
			log_w("El valor de la key a insertar excede el tamaño maximo soportado. Operacion INSERT cancelada", table_name_upper);
			free(table_name_upper);
			return false;
		} else {
			if (input->timestamp == -1) {
				input->timestamp = get_timestamp();
			}

			tkv_t* tkv = create_tkv(input);

			if (table_not_exist_in_list(mem_table, table_name_upper)) {
				//Si no existe la tabla en la mem table la creo
				table_t* new_table = create_table(table_name_upper);
				list_add(mem_table, new_table);
			}
			//Siempre busco la tabla que necesito y despues le inserto la key, por ahora sin orden despues quizas si
			table_t* table = find_table_in_list(mem_table, table_name_upper);
			list_add(table->tkvs, tkv);
			//Se les hace free cuando limpie la memetable despues.
			log_i("Se inserto satisfactoriamente la clave %d con valor %s y timestamp %lld en la tabla %s ",input->key, input->value, input->timestamp, table_name_upper);
			free(table_name_upper);
			free(table_directory);
			return true;
		}
	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion INSERT cancelada", table_name_upper);
		free(table_name_upper);
		free(table_directory);
		return false;
	}
}

void process_create(create_input_t* input) {
	log_i("fs create args: %s %i %i %ld", input->table_name, input->consistency,input->partitions, input->compaction_time);
	char* table_name_upper = to_uppercase(input->table_name);
	char* bitmap_dir = get_bitmap_directory();
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	int blocks[(input->partitions)];

	//Quiero saber si hay tantos bloques libres como particiones asi que busco cuantos bloques libres hay
	int free_blocks_amount = assign_free_blocks(bitmap, blocks, input->partitions);

	if (free_blocks_amount == input->partitions) {
		//Si habia n bloques libres me toca saber si ya existe la tabla a crear o no
		if (create_table_folder(table_name_upper) == 0) {
			//SI no existe la tabla entonces procedo con el CREATE normlamente
			log_i("Se creo el directorio de la tabla %s ", table_name_upper);

			create_table_metadata(input->consistency, input->partitions,input->compaction_time, table_name_upper);
			log_i("Se creo la metadata de la tabla %s ", table_name_upper);

			create_partitions(input->partitions, table_name_upper, blocks);
			//Guardo el bitmap
			write_bitmap(bitmap, bitmap_dir);
			log_i("Se crearon %d particiones para la tabla %s ",input->partitions, table_name_upper);
			//Crear hilo para que la tabla haga su propio dumpeo TODO
			log_i("Tabla %s creada exitosamente! ", table_name_upper);
		} else {
			log_w("La tabla %s ya esta en el sistema. Operacion CREATE cancelada.",table_name_upper);
		}

	} else {
		//SI no habia tantos bloques libres como particiones quiero tener, entonces cancelo el CREATE
		log_w("No hay bloques suficientes como para crear la tabla con %d particiones. Operacion CREATE cancelada",input->partitions);
	}

	free(table_name_upper);
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
}

t_list* process_describe(describe_input_t* input) {

	log_i("fs describe args: %s", input->table_name);
	char* table_dir = get_table_directory();
	//Si me mandan null muestro la metadata de todas las tablas
	t_list* metadata_list = list_create();
	if (input->table_name == NULL) {
		t_list* table_list = get_tables_list();

		for(int i = 0; i < table_list->elements_count; i++) {
			char* table_name = list_get(table_list , i);
			char* table_directory = create_new_directory(table_dir, table_name);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
			"\n Una consistencia del tipo %s \n Y %d particion/es.", table_name, table_metadata->compaction_time, consistency_name, table_metadata->partitions);
			free(table_directory);
			list_add(metadata_list, table_metadata);
			free(table_name);
		}

		list_destroy(table_list);
	} else {
		//Si no, me fijo si la tabla que me mando existe
		char* table_name_upper = to_uppercase(input->table_name);
		if (exist_in_directory(input->table_name, table_dir)) {
			//Si existe muestro su metadata
			char* table_directory = create_new_directory(table_dir, table_name_upper);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
					"\n Una consistencia del tipo %s \n Y %d particion/es.", table_name_upper, table_metadata->compaction_time, consistency_name, table_metadata->partitions);
			free(table_directory);
			free(table_name_upper);
			list_add(metadata_list, table_metadata);

		} else {
			//Si no existe la tabla  se termina la operacion
			log_w("La tabla %s no existe. Operacion DESCRIBE cancelada", table_name_upper);
			free(table_name_upper);

		}

	}

	free(table_dir);
	return metadata_list;
}

bool process_drop(drop_input_t* input) {
	log_i("fs drop args: %s", input->table_name);
	char* table_dir = get_table_directory();
	char* bitmap_directory = get_bitmap_directory();
	char* table_name_upper = to_uppercase(input->table_name);
	bool result;

	if (exist_in_directory(input->table_name, table_dir)) {
		char* table_directory = create_new_directory(table_dir, table_name_upper);
		t_bitarray* bitmap = read_bitmap(bitmap_directory);
		//LIbero los bloques de las particiones
		free_partitions(table_directory, bitmap);
		//Libero los bloques de todorls los tmp que existan
		free_blocks_of_all_tmps(table_directory, bitmap);
		//Borro la carpeta con todorl su contenido
		remove_directory(table_directory);
		//Guardo el bitmap
		write_bitmap(bitmap, bitmap_directory);
		log_i("La tabla %s se borro satisfactoriamente.", table_name_upper);
		result = true;
		free(bitmap->bitarray);
		free(bitmap);
		free(table_directory);
	} else {
		//Si no existe la tabla  se termina la operacion
		log_w("La tabla %s no existe. Operacion DROP cancelada", table_name_upper);
		result = false;
	}

	free(table_name_upper);
	free(table_dir);
	free(bitmap_directory);

	return result;
}


