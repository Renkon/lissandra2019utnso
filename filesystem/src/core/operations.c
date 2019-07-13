#include "operations.h"

void process_select(select_input_t* input, response_t* response) {
	log_i("fs select args: %s %u", input->table_name,(unsigned int) input->key);
	usleep(g_config.delay * 1000);
	record_t* key_found = NULL;
	char* table_name_upper = to_uppercase(input->table_name);
	is_blocked_wait(table_name_upper);
	is_blocked_post(table_name_upper);
	char* initial_table_dir = get_table_directory();
	//Primero me fijo si existe la tabla
	if (exist_in_directory(input->table_name, initial_table_dir)) {

		char* table_directory = create_new_directory(initial_table_dir,table_name_upper);
		key_found = search_key(table_directory, input->key,table_name_upper);

		if (key_found->timestamp == -1) {
			//Si la key encotnrada me da una timstamp en -1 entonces significa que no la encontro
			log_w("La clave %d no existe en la tabla %s. Operacion SELECT cancelada", input->key, table_name_upper);
			key_found->key = -1;
			key_found->value = strdup("-1");
			key_found->fs_archive_where_it_was_found = strdup("Tu, tu, tu, tu, ERES MI MEJOR AMIGO!");

		} else {
			//SI la timstamp es distinta de -1 entonces si la encontre y la muestro!
			log_i("Clave %d encontrada en la tabla %s! su valor es: %s", input->key, table_name_upper, key_found->value);
			log_i("Y la encontre en %s",key_found->fs_archive_where_it_was_found);
		}
		free(table_directory);
	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion SELECT cancelada",table_name_upper);
		// entonces instanciamos un elemento con timestamp -2 para decirle che capo no existe la tabla
		key_found = malloc(sizeof(record_t));
		key_found->key = -2;
		key_found->timestamp = -2;
		key_found->value = strdup("-2");
		key_found->fs_archive_where_it_was_found = strdup("Este es un show sobre nada... como termina?");
	}

	key_found->table_name = table_name_upper;

	free(initial_table_dir);

	if (response == NULL) {
		free(key_found->table_name);
		free(key_found->value);
		free(key_found->fs_archive_where_it_was_found);
		free(key_found);
	} else {
		set_response(response, key_found);
	}
}

void process_insert(insert_input_t* input, response_t* response) {
	log_i("fs insert args: %s %u \"%s\" %lld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	usleep(g_config.delay * 1000);
	int* insert_status = malloc(sizeof(int));
	char* table_name_upper = to_uppercase(input->table_name);
	is_blocked_wait(table_name_upper);
	is_blocked_post(table_name_upper);
	char* table_directory = get_table_directory();
	//Me fijo si existe la tabla
	if (exist_in_directory(input->table_name, table_directory)) {
		if (value_exceeds_maximun_size(input->value)) {
			log_w("El valor de la key a insertar excede el tamaño maximo soportado. Operacion INSERT cancelada", table_name_upper);
			free(table_name_upper);
			free(table_directory);
			*insert_status = -1;
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
			//Siempre busco la tabla que necesito y despues le inserto la key
			table_t* table = find_table_in_list(mem_table, table_name_upper);
			list_add(table->tkvs, tkv);
			//Se les hace free cuando limpie la memetable despues.
			log_i("Se inserto satisfactoriamente la clave %d con valor %s y timestamp %lld en la tabla %s ",input->key, input->value, input->timestamp, table_name_upper);
			free(table_name_upper);
			free(table_directory);
			*insert_status = 0;
		}
	} else {
		//Si no existe la tabla entonces se termina la operacion
		log_w("La tabla %s no existe. Operacion INSERT cancelada", table_name_upper);
		free(table_name_upper);
		free(table_directory);
		*insert_status = -2;
	}

	if (response == NULL)
		free(insert_status);
	else
		set_response(response, insert_status);
}

void process_multiinsert(t_list* inputs, response_t* response) {
	int* multiinsert_result = malloc(sizeof(int));
	*multiinsert_result = 0;

	for (int i = 0; i < list_size(inputs); i++) {
		response_t* inner_response = generate_response_object();
		process_insert((insert_input_t*) list_get(inputs, i), inner_response);
		int* result = (int*) inner_response->result;
		if (*result < 0) {
			log_w("Operacion %i de multi-insert fallo con respuesta %i", (i + 1), *result);
			*multiinsert_result = -1;
		}
		destroy_response(inner_response, INSERT_OUT);
	}

	// Response nunca sera null, porque siempre vendra de la memoria.
	set_response(response, multiinsert_result);
}

void process_create(create_input_t* input, response_t* response) {
	log_i("fs create args: %s %i %i %ld", input->table_name, input->consistency,input->partitions, input->compaction_time);
	usleep(g_config.delay * 1000);
	int* create_status = malloc(sizeof(int));
	char* table_name_upper = to_uppercase(input->table_name);
	char* bitmap_dir = get_bitmap_directory();
	sem_wait(bitmap_semaphore);
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	//Creo un array  de tantos bloques como particiones pida
	int blocks[(input->partitions)];

	//Quiero saber si hay tantos bloques libres como particiones asi que busco cuantos bloques libres hay
	//Poner semaforo todo
	int free_blocks_amount = assign_free_blocks(bitmap, blocks, input->partitions);
	if (free_blocks_amount == input->partitions) {

		//Si habia n bloques libres me toca saber si ya existe la tabla a crear o no
		if (create_table_folder(table_name_upper) == 0) {
			t_list* block_list = from_array_to_list(blocks,free_blocks_amount);
			char* blocks_to_write = get_block_string(block_list);
			//SI no existe la tabla entonces procedo con el CREATE normlamente
			log_t("Se creo el directorio de la tabla %s ", table_name_upper);

			create_table_metadata(input->consistency, input->partitions,input->compaction_time, table_name_upper);
			log_t("Se creo la metadata de la tabla %s ", table_name_upper);

			create_partitions(input->partitions, table_name_upper, blocks);
			//Guardo el bitmap
			write_bitmap(bitmap, bitmap_dir);
			add_table_to_table_state_list(table_name_upper);
			log_t("Se crearon %d particiones para la tabla %s ",input->partitions, table_name_upper);
			log_t("Se reservaron los siguientes bloques para la tabla %s: %s",table_name_upper,blocks_to_write);
			log_t("Tabla %s creada exitosamente! ", table_name_upper);
			list_destroy(block_list);
			free(blocks_to_write);
			*create_status = 0;
		} else {
			log_w("La tabla %s ya esta en el sistema. Operacion CREATE cancelada.",table_name_upper);
			*create_status = -1;
		}

	} else {
		//SI no habia tantos bloques libres como particiones quiero tener, entonces cancelo el CREATE
		log_w("No hay bloques suficientes como para crear la tabla con %d particiones. Operacion CREATE cancelada",input->partitions);
		*create_status = -2;
	}
	sem_post(bitmap_semaphore);
	free(table_name_upper);
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
	if (response == NULL)
		free(create_status);
	else
		set_response(response, create_status);
}

void process_describe(describe_input_t* input, response_t* response) {
	log_i("fs describe args: %s", input->table_name);
	usleep(g_config.delay * 1000);

	char* table_dir = get_table_directory();
	//Si me mandan null muestro la metadata de todas las tablas
	t_list* metadata_list = list_create();
	if (input->table_name == NULL) {
		t_list* table_list = get_tables_list();

		for(int i = 0; i < table_list->elements_count; i++) {
			char* table_name = list_get(table_list , i);
			char* table_directory = create_new_directory(table_dir, table_name);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			table_metadata->table_name = strdup(table_name);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
			"\n Una consistencia del tipo %s \n Y %d particion/es.", table_name, table_metadata->compaction_time, consistency_name, table_metadata->partitions);
			free(table_directory);
			list_add(metadata_list, table_metadata);
			free(table_name);
		}

		if(list_size(table_list)==0){
			log_i("No hay tablas actualmente en el filesystem.");
		}

		list_destroy(table_list);
	} else {
		//Si no, me fijo si la tabla que me mando existe
		char* table_name_upper = to_uppercase(input->table_name);
		if (exist_in_directory(input->table_name, table_dir)) {
			//Si existe muestro su metadata
			char* table_directory = create_new_directory(table_dir, table_name_upper);
			table_metadata_t* table_metadata = read_table_metadata(table_directory);
			table_metadata->table_name = strdup(table_name_upper);
			//Paso de enum a string
			char* consistency_name = get_consistency_name(table_metadata->consistency);
			log_i("La tabla %s tiene: \n Un tiempo de compactacion de %ld milisegundos "
					"\n Una consistencia del tipo %s \n Y %d particion/es.", table_name_upper, table_metadata->compaction_time, consistency_name, table_metadata->partitions);
			free(table_directory);
			list_add(metadata_list, table_metadata);
		} else {
			//Si no existe la tabla  se termina la operacion
			log_w("La tabla %s no existe. Operacion DESCRIBE cancelada", table_name_upper);
		}
		free(table_name_upper);
	}

	free(table_dir);

	if (response == NULL) {
		for (int i = 0; i < list_size(metadata_list); i++) {
			free(((table_metadata_t*) list_get(metadata_list, i))->table_name);
			free(list_get(metadata_list, i));
		}
		list_destroy(metadata_list);
	} else {
		set_response(response, metadata_list);
	}
}

void process_drop(drop_input_t* input, response_t* response) {
	log_i("fs drop args: %s", input->table_name);
	usleep(g_config.delay * 1000);
	int* drop_status = malloc(sizeof(int));
	char* table_dir = get_table_directory();
	char* bitmap_directory = get_bitmap_directory();
	char* table_name_upper = to_uppercase(input->table_name);
	is_blocked_wait(table_name_upper);
	is_blocked_post(table_name_upper);
	if (exist_in_directory(input->table_name, table_dir)) {
		char* table_directory = create_new_directory(table_dir, table_name_upper);
		sem_wait(bitmap_semaphore);
		t_bitarray* bitmap = read_bitmap(bitmap_directory);
		//LIbero los bloques de las particiones
		free_partitions(table_directory, bitmap);
		//Libero los bloques de todorls los tmp que existan
		free_blocks_of_all_tmps(table_directory, bitmap);
		//Borro la carpeta con todorl su contenido
		remove_directory(table_directory);
		//Guardo el bitmap
		write_bitmap(bitmap, bitmap_directory);
		sem_post(bitmap_semaphore);
		live_status_wait(table_name_upper);
		log_i("La tabla %s se borro satisfactoriamente.", table_name_upper);
		free(bitmap->bitarray);
		free(bitmap);
		free(table_directory);
		*drop_status = 0;
	} else {
		//Si no existe la tabla  se termina la operacion
		log_w("La tabla %s no existe. Operacion DROP cancelada", table_name_upper);
		*drop_status = -1;
	}

	free(table_name_upper);
	free(table_dir);
	free(bitmap_directory);

	if (response == NULL)
		free(drop_status);
	else
		set_response(response, drop_status);
}

void process_value(void* unused, response_t* response) {
	int* value = malloc(sizeof(int));
	memcpy(value, &(g_config.max_value_size), sizeof(int));
	set_response(response, value);
}


