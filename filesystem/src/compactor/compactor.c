#include "compactor.h"


void iniitalize_compaction_in_all_tables(){
	signal(SIGINT, handle_sigint);

	for (int i = 0; i < list_size(table_state_list); i++) {
		table_state_t* table_to_compact = list_get(table_state_list, i);
		table_to_compact->compaction_thread =initialize_compaction();
		sem_post(&thread_semaphore);
	}
}

void handle_sigint() {
	dump();
	exit(0);
}

pthread_t  initialize_compaction() {
	pthread_t compaction_thread;
	if (pthread_create(&compaction_thread, NULL, (void*) compact_this_table, NULL)) {
		log_e("No se pudo inicializar el hilo compactacion en la tabla");
	}
	return compaction_thread;
}

void compact_this_table(){
	pthread_detach(pthread_self());
	sem_wait(&thread_semaphore);
	table_state_t* table_to_compact = find_in_table_state_list_with_thread(pthread_self());

	if(table_to_compact == NULL){
		log_w("Ya se borro la tabla a compactar");
		return;
	}
	char* table_name = strdup(table_to_compact->name);
	char* initial_table_dir = get_table_directory();
	char* table_directory = create_new_directory(initial_table_dir,table_name);
	table_metadata_t* table_metadata = read_table_metadata(table_directory);
	int compaction_time = table_metadata->compaction_time;
	free(initial_table_dir);
	free(table_directory);
	free(table_metadata);
	log_t("Hilo de compactacion de la tabla %s iniciado",table_name);
	while(get_live_status(pthread_self()) == 1){
		compaction(table_name);
		usleep(compaction_time * 1000);
	}
	log_t("Hilo de compactacion de la tabla %s finalizado",table_name);
	free(table_name);
	destroy_in_table_state_list(pthread_self());
}


void compaction(char* table_name) {
	char* initial_table_dir = get_table_directory();
	char* table_directory = create_new_directory(initial_table_dir, table_name);
	int tmp_number = 1;
	char* tmp_name = get_tmp_name(tmp_number);
	char* tmpc_directory = get_tmpc_directory(table_directory);
	if (exist_in_directory(table_name, initial_table_dir)) {
		table_metadata_t* table_metadata = read_table_metadata(table_directory);

		if (exist_in_directory(get_tmpc_name, table_directory)) {
			partition_t* tmpc = read_fs_archive(tmpc_directory);
			log_t("Se leyo A1.tmpc de la tabla %s. Iniciando compactacion del mismo.",table_name);
			start_compaction(table_directory, table_metadata, tmpc,tmpc_directory, table_name, 0);

		} else {
			if (exist_in_directory(tmp_name, table_directory)) {
				partition_t* tmpc = get_all_blocks_from_all_tmps(table_name);
				//Creo el tmpc
				create_fs_archive(table_name, tmpc->blocks,tmpc->number_of_blocks, tmpc->size, 0, 0);
				log_t("Se creo el archivo A1.tmpc en la tabla %s", table_name);
				start_compaction(table_directory, table_metadata, tmpc,tmpc_directory, table_name, 1);
			}
		}
		free(table_metadata);
	} else {
		log_w("Error al intentar compactar. La tabla %s no existe en el sistema",table_name);

	}

	free(tmpc_directory);
	free(tmp_name);
	free(initial_table_dir);
	free(table_directory);
}

void start_compaction(char* table_directory,table_metadata_t* table_metadata,partition_t* tmpc, char* tmpc_directory, char* table_name, int tmp_destroy_flag){
	long long first_timestamp;
	long long last_timestamp;
	long long bloqued_time;
	t_list* tmpc_tkvs = create_tkv_list(tmpc);
	t_list* partition_tkvs = create_partition_tkv_list(table_directory,table_metadata);
	get_tkvs_to_insert(tmpc_tkvs,partition_tkvs);
	list_destroy_and_destroy_elements(tmpc_tkvs,free_record);
	is_blocked_wait(table_name); //bloque la table
	first_timestamp = get_timestamp();
	sem_wait(bitmap_semaphore);
	char* bitmap_dir = get_bitmap_directory();
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	free_partitions(table_directory, bitmap);
	if(tmp_destroy_flag == 1){
		free_blocks_of_all_tmps(table_directory, bitmap);
		destroy_all_tmps(table_directory);
	}
	write_bitmap(bitmap, bitmap_dir);
	sem_post(bitmap_semaphore);
	remove(tmpc_directory);
	create_new_partitions(partition_tkvs,table_name);
	is_blocked_post(table_name);// desbloqueo
	last_timestamp = get_timestamp();
	bloqued_time =last_timestamp-first_timestamp;
	log_i("Compactacion terminada sobre la tabla %s!", table_name);
	log_i("Y estuvo bloqueada un total de %lld milisegundos", bloqued_time);
	list_destroy(partition_tkvs);
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
	free(tmpc->blocks);
	free(tmpc);

}

char* get_block_string(t_list* block_list){
	char* block_string = NULL;
	for (int i = 0; i < list_size(block_list); i++) {
		int block = list_get(block_list, i);
		if (block_string == NULL) {
			block_string = malloc(digits_in_a_number(block) + 1);
			sprintf(block_string, "%d", block);
		} else {
			char* next_block = malloc(digits_in_a_number(block) + 3);
			sprintf(next_block, ", %d", block);
			string_append(&block_string, next_block);
			free(next_block);
		}

	}
	if(block_string == NULL){
		block_string = strdup("Ninguno");
	}
	return block_string;
}

int* from_list_to_array(t_list* list){

	int* array = malloc((sizeof(int))*(list_size(list)));
	for(int i=0; i<list_size(list);i++){
		array[i] = list_get(list,i);
	}
	return array;
}

t_list* from_array_to_list (int* array, int size){
	t_list* list = list_create();
	for(int i=0;i<size;i++){
		int array_inindex = array[i];
		list_add(list,array_inindex);
	}
	return list;
}

void destroy_all_tmps(char* table_directory){
	int tmp_number = 1;
	char* tmp_name = get_tmp_name(tmp_number);
	while (exist_in_directory(tmp_name, table_directory)) {
		char* tmp_dir = get_tmp_directory(table_directory, tmp_number);
		remove(tmp_dir);
		free(tmp_dir);
		tmp_number++;
		free(tmp_name);
		tmp_name = get_tmp_name(tmp_number);
	}
	free(tmp_name);



}

int add_blocks_for_partitions_without_tkvs(t_list* partition_tkvs) {
	//Vas a pedir un bloque mas por cada particion que no tenga tkvs.
	int blocks = 0;

	for (int i = 0; i < list_size(partition_tkvs); i++) {
		tkvs_per_partition_t* partition = list_get(partition_tkvs, i);

		if (list_size(partition->tkvs) == 0) {
			blocks += 1;
		}
	}
	return blocks;
}

tkv_t* convert_to_tkv(record_t* record){
	tkv_t* tkv= malloc(sizeof(tkv_t));
	tkv->tkv = malloc(digits_in_a_number(record->timestamp)+digits_in_a_number(record->key)+strlen(record->value)+3);
	sprintf(tkv->tkv,"%lld;%d;%s",record->timestamp,record->key,record->value);
	return tkv;
}

int length_needed_to_add_tkvs_in_partitions(t_list* partition_tkvs){
	int total_length =0;
	for(int i=0; i<list_size(partition_tkvs); i++){
			tkvs_per_partition_t* partition = list_get(partition_tkvs,i);
				t_list* string_tkv_list = list_map(partition->tkvs,convert_to_tkv);
				total_length+= necessary_blocks_for_tkvs(string_tkv_list);
				list_destroy_and_destroy_elements(string_tkv_list,free_tkv);
		}

	return total_length;
}

void create_partition(tkvs_per_partition_t* partition,char* table_name) {
	t_list* blocks = list_create();
	char* bitmap_dir = get_bitmap_directory();
	int  the_block;
	sem_wait(bitmap_semaphore);
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	the_block = find_free_block(bitmap)+1;
	write_bitmap(bitmap, bitmap_dir);
	sem_post(bitmap_semaphore);
	log_t("Se escribio el bloque %d para la compactacion de la tabla %s",the_block,table_name);
	free(bitmap->bitarray);
	free(bitmap);
	list_add(blocks,the_block);
	int block_size = fs_metadata->block_size;
	FILE* block = open_block(the_block);

	for (int i = 0; i < list_size(partition->tkvs); i++) {
		record_t* tkv = list_get(partition->tkvs, i);
		tkv_t* readed_tkv = convert_to_tkv(tkv);
		int free_space_in_block = block_size - strlen(readed_tkv->tkv);
		if (free_space_in_block > 0) {
			//Si me entra el tkv en el bloque lo meto asi nomas.
			block_size-=strlen(readed_tkv->tkv);
			write_tkv(readed_tkv->tkv, block);
		} else {
			//Si no entra va al la segunda logica de como guardar un tkv
			int what_the_pionter_moved = 0;
			//Como voy a mover el puntero del string tengo que saber cuanto se va moviendo para despes dejarlo en 0
			// Y podes hacerle free en paz.
			while (true) {
				//Me fijo si el string entra en el bloque
				int free_space_in_block2 = block_size - strlen(readed_tkv->tkv);
				if (free_space_in_block2 >= 0) {
					//SI entra lo guardo directamente y corto
					char* tkv_to_write = malloc(strlen(readed_tkv->tkv)+1);
					strcpy(tkv_to_write,readed_tkv->tkv);
					write_tkv(tkv_to_write, block);
					block_size-=strlen(readed_tkv->tkv);
					free(tkv_to_write);
					break;
				}
				//SI no entra entonces guardo lo que si entra con un \n al final y vuelvo a intentar con un nuevo bloque
				char* tkv_that_enters = string_substring_until(readed_tkv->tkv,block_size - 1);
				readed_tkv->tkv += block_size - 1;
				what_the_pionter_moved += block_size - 1;
				string_append(&tkv_that_enters, "\n");
				write_tkv(tkv_that_enters,block);
				free(tkv_that_enters);
				fclose(block);
				sem_wait(bitmap_semaphore);
				t_bitarray* bitmap = read_bitmap(bitmap_dir);
				the_block = find_free_block(bitmap)+1;
				write_bitmap(bitmap, bitmap_dir);
				sem_post(bitmap_semaphore);
				log_t("Se escribio el bloque %d para la compactacion de la tabla %s",the_block,table_name);
				free(bitmap->bitarray);
				free(bitmap);
				list_add(blocks,the_block);
				block = open_block(the_block);
				block_size = fs_metadata->block_size;
			}

			readed_tkv->tkv-= what_the_pionter_moved;
		}
		//Si mi bloque se lleno o quedo con 1 solo carcter entonces lo cierro y paso al siguiente
		if (block_size <= 1) {
			fclose(block);
			sem_wait(bitmap_semaphore);
			t_bitarray* bitmap = read_bitmap(bitmap_dir);
			the_block = find_free_block(bitmap)+1;
			write_bitmap(bitmap, bitmap_dir);
			sem_post(bitmap_semaphore);
			log_t("Se escribio el bloque %d para la compactacion de la tabla %s",the_block,table_name);
			free(bitmap->bitarray);
			free(bitmap);
			list_add(blocks,the_block);
			block = open_block(the_block);
			block_size = fs_metadata->block_size;
		}
		free_tkv(readed_tkv);
	}
	fclose(block);
	int* blocks_array = from_list_to_array(blocks);
	t_list* string_tkv_list = list_map(partition->tkvs,convert_to_tkv);
	int size_of_all_tkvs_from_partition = size_of_all_tkvs(string_tkv_list);
	create_fs_archive(table_name,blocks_array,list_size(blocks),size_of_all_tkvs_from_partition,2,partition->partition);
	list_destroy(blocks);
	list_destroy_and_destroy_elements(string_tkv_list,free_tkv);
	free(blocks_array);
	free(bitmap_dir);
}

void create_new_partitions(t_list* partition_tkvs,char*  table_name){
	for(int i=0; i<list_size(partition_tkvs);i++){
		//ver_bloques(blocks,blocks_length);
		tkvs_per_partition_t* partition = list_get(partition_tkvs,i);
		create_partition(partition,table_name);
		free_tkvs_per_partition(partition);
	}

}


void get_tkvs_to_insert(t_list* tmpc_tkvs, t_list* partition_tkvs){

	for(int i=0; i<list_size(tmpc_tkvs); i++){
		record_t* record = list_get(tmpc_tkvs,i);
		int partition_number = (record->key % list_size(partition_tkvs)) + 1;
		tkvs_per_partition_t* partition = list_get(partition_tkvs,partition_number-1);
		add_record_to_partition_list(record,partition);

	}


}

void add_record_to_partition_list (record_t* record,tkvs_per_partition_t* partition){

	for(int i=0; i<list_size(partition->tkvs);i++){
		record_t* record_from_partition = list_get(partition->tkvs,i);

		if(record->key == record_from_partition->key){
			//Si lo encuentra y la timestamp es menor entonces lo agrego
			if(record->timestamp > record_from_partition->timestamp){
				/*free(record_from_partition->value);
				free(record_from_partition);*/
				list_remove_and_destroy_element(partition->tkvs,i,free_record);
				record_t* the_record = copy_key(record);
				list_add(partition->tkvs, the_record);
				return;
			}
		//Si tiene la misma key pero menos timestamp no hago nada
		return;
		}

	}
	//Si no lo encuentra entronces lo agrega al final
	record_t* the_record = copy_key(record);
	list_add(partition->tkvs, the_record);
}

tkv_t* add_records_from_block(int block, int index, int incomplete_tkv_size,t_list* tkvs) {
	char* block_directory = create_block_directory(block);
	tkv_t* key_found_in_block = malloc(sizeof(tkv_t));
	key_found_in_block->tkv = malloc(tkv_size());
	//Le seteo -1 para que si no la encuentra, devuelva este "tkv invalido"
	strcpy(key_found_in_block->tkv, "-1;-1;-1");
	long long key_found_timestamp = -1;
	key_found_in_block->incomplete = false;
	FILE* arch = fopen(block_directory, "rb");

	char* readed_key = calloc(1, tkv_size());

	//SI mando index en 1 me salteo el primer read
	//Porque asi leo la parte del tkv anterior que ya lei
	int pointer= 0;
	if (index == 1) {
		fread(readed_key, 1, incomplete_tkv_size, arch);
		pointer+= incomplete_tkv_size;
	}

	int i = 0;
	while (!feof(arch)) {
		size_t lecture = fread(readed_key, 1, tkv_size(), arch);
		pointer+= strlen(readed_key)+1;
		fseek(arch,pointer,SEEK_SET);
		if (readed_key[0] && string_ends_with(readed_key, "\n")) {
			//Si tiene \n entonces copio este string sin el \n y prengo el flag de incompleto
			char* substr = string_substring_until(readed_key, strlen(readed_key) - 1);
			strcpy(key_found_in_block->tkv, substr);
			key_found_in_block->incomplete = true;
			free(substr);
			break;
		}

		if (lecture == 0) {
			//Si el bloque no tiene nada entonces corto todorl.
			break;
		}

		record_t* record = convert_record(readed_key);
		list_add(tkvs, record);
	}

	fclose(arch);
	free(readed_key);
	free(block_directory);
	return key_found_in_block;

}

partition_t* get_all_blocks_from_all_tmps (char* table_name){
	char* initial_table_dir = get_table_directory();
	char* table_directory = create_new_directory(initial_table_dir,table_name);
	int tmp_number = 1;
	char* tmp_name = get_tmp_name(tmp_number);
	int all_blocks_size = 0;
	int all_tkv_size=0;
	int* all_blocks = malloc(sizeof(int));
	partition_t* tmpc = malloc(sizeof(partition_t));

	while (exist_in_directory(tmp_name, table_directory)) {
		char* tmp_dir = get_tmp_directory(table_directory, tmp_number);
		partition_t* partition = read_fs_archive(tmp_dir );
		all_blocks =  int_array_concat(all_blocks,all_blocks_size,partition->blocks,partition->number_of_blocks);
		all_blocks_size+=partition->number_of_blocks;
		all_tkv_size += partition->size;
		tmp_number++;
		free(tmp_dir);
		free(tmp_name);
		free(partition->blocks);
		free(partition);
		tmp_name = get_tmp_name(tmp_number);
	}

	tmpc->number_of_blocks = all_blocks_size;
	tmpc->size = all_tkv_size;
	tmpc->blocks = malloc(sizeof(int)*all_blocks_size);
	memcpy(tmpc->blocks,all_blocks,sizeof(int)*all_blocks_size);
	free(tmp_name);
	free(initial_table_dir);
	free(table_directory);
	free(all_blocks);
	return tmpc;
}


void initialize_dump() {
	pthread_t dump_thread;
	if (pthread_create(&dump_thread, NULL, (void*) dump_all_tables, NULL)) {
		log_e("No se pudo inicializar el hilo de dump");
	}
}

void dump_all_tables(){
	pthread_detach(pthread_self());
	log_t("Iniciado el hilo de dump");
	while(true){
		dump();
		usleep(g_config.dump_time * 1000);
	}
}

void dump() {
	sem_wait(&dump_semaphore);
	if (mem_table->elements_count > 0) {

		for (int i = 0; i < mem_table->elements_count; i++) {
			table_t* table = list_get(mem_table, i);
			dump_table(table);
			log_i("Dumpeados los datos de la tabla %s", table->name);
		}
	}
	free_memtable();
	sem_post(&dump_semaphore);
}


void free_memtable(){
	//Destruyo la lista y todos sus elementos
	for(int i=0;i<mem_table->elements_count;i++){
		table_t* table =list_get(mem_table,i);
		free_table(table);

	}
	list_destroy(mem_table);
	mem_table = list_create();
	//Al final creo de nuevo la memtable
}

void dump_table(table_t* table){
	t_list* blocks = list_create();
	char* bitmap_dir = get_bitmap_directory();
	int the_block;
	sem_wait(bitmap_semaphore);
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	the_block = find_free_block(bitmap)+1;
	write_bitmap(bitmap, bitmap_dir);
	sem_post(bitmap_semaphore);
	log_t("Se escribio el bloque %d para la compactacion de la tabla %s",the_block,table->name);
	free(bitmap->bitarray);
	free(bitmap);
	list_add(blocks,the_block);
	int block_size = fs_metadata->block_size;
	//Abro el .bin del primer bloque
	FILE* block = open_block(the_block);
	for (int i = 0; i < table->tkvs->elements_count; i++) {
		tkv_t* readed_tkv = list_get(table->tkvs, i);
		int free_space_in_block = block_size - strlen(readed_tkv->tkv);
		if (free_space_in_block > 0) {
			//Si me entra el tkv en el bloque lo meto asi nomas.
			block_size-=strlen(readed_tkv->tkv);
			write_tkv(readed_tkv->tkv, block);
		} else {
			//Si no entra va al la segunda logica de como guardar un tkv
			int what_the_pionter_moved = 0;
			//Como voy a mover el puntero del string tengo que saber cuanto se va moviendo para despes dejarlo en 0
			// Y podes hacerle free en paz.
			while (true) {
				//Me fijo si el string entra en el bloque
				int free_space_in_block2 = block_size - strlen(readed_tkv->tkv);
				if (free_space_in_block2 >= 0) {
					//SI entra lo guardo directamente y corto
					char* tkv_to_write = malloc(strlen(readed_tkv->tkv)+1);
					strcpy(tkv_to_write,readed_tkv->tkv);
					write_tkv(tkv_to_write, block);
					block_size-=strlen(readed_tkv->tkv);
					free(tkv_to_write);
					break;
				}
				//SI no entra entonces guardo lo que si entra con un \n al final y vuelvo a intentar con un nuevo bloque
				char* tkv_that_enters = string_substring_until(readed_tkv->tkv,block_size - 1);
				readed_tkv->tkv += block_size - 1;
				what_the_pionter_moved += block_size - 1;
				string_append(&tkv_that_enters, "\n");
				write_tkv(tkv_that_enters,block);
				fclose(block);
				free(tkv_that_enters);
				sem_wait(bitmap_semaphore);
				t_bitarray* bitmap = read_bitmap(bitmap_dir);
				the_block = find_free_block(bitmap)+1;
				write_bitmap(bitmap, bitmap_dir);
				sem_post(bitmap_semaphore);
				log_t("Se escribio el bloque %d para la compactacion de la tabla %s",the_block,table->name);
				free(bitmap->bitarray);
				free(bitmap);
				list_add(blocks,the_block);
				block = open_block(the_block);
				block_size = fs_metadata->block_size;
			}
			readed_tkv->tkv-= what_the_pionter_moved;
		}
		//Si mi bloque se lleno o quedo con 1 solo carcter entonces lo cierro y paso al siguiente
		if (block_size <= 1) {
			fclose(block);
			sem_wait(bitmap_semaphore);
			t_bitarray* bitmap = read_bitmap(bitmap_dir);
			the_block = find_free_block(bitmap)+1;
			write_bitmap(bitmap, bitmap_dir);
			sem_post(bitmap_semaphore);
			log_t("Se escribio el bloque %d para el dumpeo de la tabla %s",the_block,table->name);
			free(bitmap->bitarray);
			free(bitmap);
			list_add(blocks,the_block);
			block = open_block(the_block);
			block_size = fs_metadata->block_size;
		}

	}
	fclose(block);
	int* blocks_array = from_list_to_array(blocks);
	int size_of_all_tkvs_in_table = size_of_all_tkvs(table->tkvs);
	create_fs_archive(table->name,blocks_array,list_size(blocks),size_of_all_tkvs_in_table,1,0);
	list_destroy(blocks);
	free(bitmap_dir);
	free(blocks_array);

}




