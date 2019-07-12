#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char* table_dir = get_table_directory();
	char* table_directory = create_new_directory(table_dir, table_name);
	int dir = mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	free(table_dir);
	free(table_directory);
	//Esta funcion es la que crea la carpeta
	return dir;
	//mkdir devuelve 0 si creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name, int* blocks) {
	char* table_dir = get_table_directory();
	char* table_directory = create_new_directory(table_dir,table_name);

	for (int i = 0; i < partitions; i++) {
		//Pongo i+1 porque no existe la particion "0"
		char* bin_name = malloc(digits_in_a_number(i+1) + strlen("/.bin") + 1);
		sprintf(bin_name, "/%d.bin", i + 1); //Esto transforma de int a string
		char* partition_route = malloc(strlen(table_directory) + strlen(bin_name) + 1);
		strcpy(partition_route, table_directory);
		strcat(partition_route, bin_name); //aca tengo toda la direccion y termina con n.bin n= nombre archivo

		FILE* arch = fopen(partition_route, "wb");
		//Creo el struct de la particion con sus datos
		partition_t* partition = malloc(sizeof(partition_t));
		partition->number_of_blocks = 1;
		partition->blocks = malloc(sizeof(int));
		partition->blocks[0] = blocks[i];
		partition->size = 0;

		fwrite(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks), arch);
		fwrite(partition->blocks, 1, sizeof(int), arch);
		fwrite(&partition->size, 1, sizeof(partition->size), arch);

		fclose(arch);
		free(bin_name);
		free(partition->blocks);
		free(partition);
		free(partition_route);
	}
	free(table_directory);
	free(table_dir);
}

void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name) {
	char* table_dir = get_table_directory();
	char* table_directory = create_new_directory(table_dir,table_name);
	char* metadata_name = "/metadata.bin";
	char* metadata_directory = create_metadata_directory(table_directory);
	//con esto ya tendria toda la direccion donde va a estar la metadata
	//Creo el struct metadata a guardar con los datos del input
	table_metadata_t* table_metadata = create_metadata(consistency, partitions, compaction_time);
	//ABro el nuevo .bin y le guardo los datos correspondientes
	FILE* arch = fopen(metadata_directory, "wb");
	fwrite(&table_metadata->compaction_time, 1,sizeof(table_metadata->compaction_time), arch);
	fwrite(&table_metadata->consistency, 1, sizeof(table_metadata->consistency),arch);
	fwrite(&table_metadata->partitions, 1, sizeof(table_metadata->partitions),arch);
	fclose(arch);
	free(table_directory);
	free(metadata_directory);
	free(table_dir);
	free(table_metadata);
}

table_metadata_t* create_metadata(consistency_t consistency, int partitions, long compaction_time) {
	table_metadata_t* metadata = malloc(sizeof(table_metadata_t));
	metadata->compaction_time = compaction_time;
	metadata->consistency = consistency;
	metadata->partitions = partitions;
	return metadata;
}

int assign_free_blocks(t_bitarray* bitmap, int* blocks, int partitions_amount) {
	int blocks_amount = 0;
	//Me fijo que la cantidad de particiones que me pedis sea menor a la cantidad de bloques totales
	if (partitions_amount <= bitmap->size) {
		//Voy a buscar bloques tantas veces como particiones vaya a tener mi tabla
		for (int i = 0; i < partitions_amount; i++) {
			int block = find_free_block(bitmap);
			if (block == -1) {
				//SI no encontre un bloque libre se termina
				break;
			} else {
				//Si encontre un bloque lo guardo en mi array de bloques y aumento el contador de bloques
				blocks_amount++;
				blocks[i] = block+1;
			}
		}
	}
	//Siempre voy  a devolver la cantidad de bloques libres que encontre
	return blocks_amount;
}

int find_free_block(t_bitarray* bitmap) {
	int i = 0;
	//Busco por todorl el bitmap hasta encontrar un 0

	while (i < bitmap->size) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			//Si lo encuentro seteo ese lugar en 1 y lo devuelvo
			bitarray_set_bit(bitmap, i);
			return i;
		}
		i++;
	}
	//Si no lo encuentro solo devuelvo -1
	return -1;
}

record_t* search_key(char* table_directory, int key, char* table_name){
	is_blocked_wait(table_name);
	is_blocked_post(table_name);
	//Busco la key en el unico archivo tmp que puede haber
	record_t* key_found_in_tmpc = search_in_tmpc(table_directory, key);
	//Busco la key en todos los tmps que existan
	is_blocked_wait(table_name);
	is_blocked_post(table_name);
	record_t* key_found_in_tmp = search_in_all_tmps(table_directory, key);
	//Busco la key en la particion que deberia estar
	is_blocked_wait(table_name);
	is_blocked_post(table_name);
	record_t* key_found_in_partition = search_in_partition(table_directory, key);
	is_blocked_wait(table_name);
	is_blocked_post(table_name);
	//Busco la key en la memtable
	record_t* key_found_in_memtable = search_key_in_memtable(key,table_name);
	//Comparo las 3 keys y por transitividad saco la que tiene la timestamp mas grande
	record_t* auxiliar_key = key_with_greater_timestamp(key_found_in_tmp, key_found_in_tmpc);
	record_t* auxiliar_key2 = key_with_greater_timestamp(auxiliar_key, key_found_in_memtable);
	record_t* most_current_key = key_with_greater_timestamp(auxiliar_key2, key_found_in_partition);
	//Devuelvo lo que encontre, si no esta la key entonces devuelvo una key con timestamp en -1
	record_t* the_key = copy_key(most_current_key);

	free(key_found_in_memtable->value);
	free(key_found_in_memtable);
	free(key_found_in_tmpc->value);
	free(key_found_in_tmpc);
	free(key_found_in_tmp->value);
	free(key_found_in_tmp);
	free(key_found_in_partition->value);
	free(key_found_in_partition);

	return the_key;

}

table_t* search_table_in_memtable(char* table_name) {
	table_t* table_not_found = malloc(sizeof(table_t));
	table_not_found->name = "Not_found";
	for (int i = 0; i < list_size(mem_table); i++) {
		table_t* table = list_get(mem_table, i);
		if (string_equals_ignore_case(table->name, table_name)) {
			free(table_not_found);
			return table;
		}
	}
	return table_not_found;
}

record_t* search_key_in_memtable(int key, char* table_name){
	record_t* key_found = malloc(sizeof(record_t));
	key_found->timestamp = -1;
	key_found->value = malloc(1);
	table_t* table = search_table_in_memtable(table_name);
	if (!string_equals_ignore_case(table->name,"Not_found")){

		for (int i = 0; i < list_size(table->tkvs); i++) {
			tkv_t* tkv_found = list_get(table->tkvs,i);
			record_t* record_found = convert_record(tkv_found->tkv);

			if(record_found->key == key && record_found->timestamp > key_found->timestamp){
				free(key_found->value);
				free(key_found);
				key_found = record_found;
			}

		}

	}

	return key_found;

}


record_t* search_in_tmpc(char* table_directory, int key) {
	record_t* key_found;
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"

	if (exist_in_directory(get_tmpc_name, table_directory)) {
		char* tmpc_dir = get_tmpc_directory(table_directory);
		key_found = search_key_in_fs_archive(tmpc_dir, key);
		free(tmpc_dir);
	} else {
		key_found = malloc(sizeof(record_t));
		key_found->value = NULL;
		key_found->timestamp = -1;
	}

	//Al final devuelvo la key que encontre si es que habia o la key default con timestamp -1 si no estaba
	return key_found;
}


record_t* search_in_all_tmps(char* table_directory, int key) {
	//ESta es la key que voy a devolver al final
	record_t* key_found_in_tmp = malloc(sizeof(record_t));
	key_found_in_tmp->value = malloc(1);
	key_found_in_tmp->timestamp = -1;

	//ESta key es para meter las keys que encuentro en cada tmp
	record_t* key_found;
	//Los nombres de los tmp empiezan desde el 1
	int tmp_number = 1;
	char* tmp_name = get_tmp_name(tmp_number);

	while (exist_in_directory(tmp_name, table_directory)) {
		char* tmp_dir = get_tmp_directory(table_directory, tmp_number);
		key_found = search_key_in_fs_archive(tmp_dir, key);

		if(key_found->timestamp > key_found_in_tmp->timestamp){
			free(key_found_in_tmp->value);
			free(key_found_in_tmp);
			//Si la key que encontre tienen mas timestamp que la key que habia encontrado antes o la default
			//Entonces la guardo porque es la mas actual
			key_found_in_tmp = copy_key(key_found);
		}

		tmp_number++;
		free(key_found->value);
		free(key_found);
		free(tmp_dir);
		free(tmp_name);
		tmp_name = get_tmp_name(tmp_number);
	}

	free(tmp_name);

	//Al final devuelvo la key con timestamp mas actual si la encontre o la key default con timestamp -1 si no
	return key_found_in_tmp;
}

record_t* search_in_partition(char* table_directory, int key) {
	//Leo la metadata de la tabla
	table_metadata_t* table_metadata = read_table_metadata(table_directory);
	int partition_number = (key % table_metadata->partitions) + 1;
	char* partition_dir = create_partition_directory(table_directory, partition_number);
	//De nuevo la key default con timestamp en -1
	record_t* key_found = search_key_in_fs_archive(partition_dir, key);

	free(partition_dir);
	free(table_metadata);
	return key_found;
}

record_t* create_tkv(insert_input_t* input) {
	tkv_t* tkv = malloc(sizeof(tkv_t));
	tkv->tkv = malloc(digits_in_a_number(input->timestamp) + digits_in_a_number(input->key) + strlen(input->value) + 3);
	sprintf(tkv->tkv, "%lld;%d;%s", input->timestamp, input->key, input->value);
	return tkv;
}

void free_tkv(tkv_t* tkv){
	free(tkv->tkv);
	free(tkv);
}

void free_table(table_t* table){
	free(table->name);
	for(int i=0; i<table->tkvs->elements_count;i++){
		tkv_t* tkv = list_get(table->tkvs,i);
		free_tkv(tkv);

	}
	list_destroy(table->tkvs);
	free(table);
}
bool value_exceeds_maximun_size(char* value){
	return strlen(value) > g_config.max_value_size;
}

void free_partitions(char* table_directory, t_bitarray* bitmap) {
	table_metadata_t* table_metadata = read_table_metadata(table_directory);

	for(int i = 0; i < table_metadata->partitions; i++){
		char* partition_directory = create_partition_directory(table_directory, i + 1);
		free_blocks_of_fs_archive(partition_directory, bitmap);
		free(partition_directory);
	}

	free(table_metadata);
}

void free_blocks_of_fs_archive(char* archive_directory, t_bitarray* bitmap) {
	partition_t* partition = read_fs_archive(archive_directory);

	for(int i = 0; i < partition->number_of_blocks; i++){
		free_block(partition->blocks[i]);
		//LIbero el espacio en el bitarray asi lo puedo usar en una nueva tabla
		bitarray_clean_bit(bitmap, (partition->blocks[i] - 1));

	}
	free(partition->blocks);
	free(partition);
}

void free_block(int block) {
	char* block_directory = create_block_directory(block);
	//Borro el bloque
	remove(block_directory);
	//Creo otra vez el mismo bloque pero  como no le meto nada queda vacio
	FILE* arch = fopen(block_directory, "wb");
	fclose(arch);
	free(block_directory);

}


void free_blocks_of_all_tmps(char* table_directory, t_bitarray* bitmap) {
	int tmp_number = 1;

	//Libero todos los bloques que tengan los tmp que existan
	char* tmp_name = get_tmp_name(tmp_number);

	while (exist_in_directory(tmp_name, table_directory)) {
		char* tmp_dir = get_tmp_directory(table_directory, tmp_number++);
		free_blocks_of_fs_archive(tmp_dir, bitmap);
		free(tmp_dir);
		free(tmp_name);
		tmp_name = get_tmp_name(tmp_number);
	}

	free(tmp_name);
}

record_t* convert_record(char* tkv_string){
	record_t* record = malloc(sizeof(record_t));
	char** tkv = string_split(tkv_string, ";");
	record->value = malloc(strlen(tkv[2])+1);
	strcpy(record->value,tkv[2]);
	 record->key =string_to_uint16(tkv[1]);
	 record->timestamp = string_to_long_long(tkv[0]);
	 free(tkv[0]);
	 free(tkv[1]);
	 free(tkv[2]);
	 free(tkv);
	 return record;
}

void free_record(record_t* record){
	free(record->value);
	free(record);
}

void add_table_to_table_state_list(char* table_name){
	table_state_t* table_state = malloc(sizeof(table_state_t));
	table_state->is_blocked_mutex = malloc(sizeof(sem_t));
	sem_init(table_state->is_blocked_mutex, 0,1);
	table_state->live_status_mutex = malloc(sizeof(sem_t));
	sem_init(table_state->live_status_mutex, 0,1);
	char* upper_read = to_uppercase(table_name);
	table_state->name = strdup(upper_read);
	list_add(table_state_list, table_state);
	table_state->compaction_thread =initialize_compaction();
	free(upper_read);
}

table_state_t* find_in_table_state_list(char* table_name) {
	table_state_t* table_find_failed = malloc(sizeof(table_state_t));
	table_find_failed->name = "failed";

	for (int i = 0; i < list_size(table_state_list); i++) {
		table_state_t* table_found = list_get(table_state_list, i);
		char* table_found_name = table_found->name;
		if (string_equals_ignore_case(table_found_name, table_name)) {
			free(table_find_failed);
			return table_found;
		}
	}
log_w("No se encontro la tabla %s", table_name);
return table_find_failed;
}

table_state_t* find_in_table_state_list_with_thread(pthread_t thread){
	table_state_t* table_find_failed = malloc(sizeof(table_state_t));
	table_find_failed->name = "failed";

	for (int i = 0; i < list_size(table_state_list); i++) {
		table_state_t* table_found = list_get(table_state_list, i);
		pthread_t table_found_thread = table_found->compaction_thread;
		if (table_found_thread == thread) {
			free(table_find_failed);
			return table_found;
		}
	}
log_w("No se encontro la tabla con ese thread");
return table_find_failed;
}


void destroy_in_table_state_list(char* table_name) {

	for (int i = 0; i < list_size(table_state_list); i++) {
		table_state_t* table_found = list_get(table_state_list, i);
		char* table_found_name = table_found->name;
		if (string_equals_ignore_case(table_found_name, table_name)) {
			free(table_found ->is_blocked_mutex);
			free(table_found ->live_status_mutex);
			free(table_found ->name);
			free(table_found);
			list_remove(table_state_list,i);
			return;
		}
	}

}


void is_blocked_wait(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	sem_wait(the_table->is_blocked_mutex);
}


void is_blocked_post(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	sem_post(the_table->is_blocked_mutex);
}

void live_status_wait(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	sem_wait(the_table->live_status_mutex);
}

void live_status_post(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	sem_post(the_table->live_status_mutex);
}

int* get_live_status(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	int* semaphore;
	 sem_getvalue(the_table->live_status_mutex,&semaphore);
	 return semaphore;
}

void eliminate_table_form_table_state_list(char* table_name){
	table_state_t* the_table = find_in_table_state_list(table_name);
	free(the_table->is_blocked_mutex);
	free(the_table->live_status_mutex);
	free(the_table->name);


}


