#include "compactor.h"

void compaction(char* table_name){
	char* initial_table_dir = get_table_directory();
	char* table_directory = create_new_directory(initial_table_dir,table_name);
	table_metadata_t* table_metadata = read_table_metadata(table_directory);
	partition_t* tmpc =get_all_blocks_from_all_tmps(table_name);
	//Creo el tmpc
	create_fs_archive(table_name,tmpc->blocks,tmpc->number_of_blocks,tmpc->size,0,0);
	//Matar todos los tmps todo;
	t_list* tmpc_tkvs = create_tkv_list(tmpc);
	t_list* partition_tkvs = create_partition_tkv_list(table_directory,table_metadata);
	get_tkvs_to_insert(tmpc_tkvs,partition_tkvs);
	//Bloqueo tablas todo
	//Libera bloques de los tmpc y el bin todo
	//t_list* partion_tkvs_string_form = list_map(partition_tkvs,transform_records_to_tkv);
	int necessary_blocks= length_needed_to_add_tkvs_in_partitions(partition_tkvs);//SACA MAS BLOQUES DE LOS QUE NECESITA TODO
	necessary_blocks += add_blocks_for_partitions_without_tkvs(partition_tkvs);
	int* blocks= malloc(sizeof(int)*necessary_blocks);
	char* bitmap_dir = get_bitmap_directory();
	sem_wait(bitmap_semaphore);
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	int free_blocks_amount = assign_free_blocks(bitmap, blocks, necessary_blocks);
	create_new_partitions(partition_tkvs,blocks,free_blocks_amount,table_name);
	//Desbloqueo tablas todo
	write_bitmap(bitmap, bitmap_dir);
	sem_post(bitmap_semaphore);
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
	free(tmpc->blocks);
	free(tmpc);
	free(initial_table_dir);
	free(table_directory);
	free(table_metadata);
	//free la tmpc_tkvs y partition tkvs todo
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
				t_list* string_tkv_list = list_map(partition->tkvs,convert_to_tkv); //hacer free TODO
				total_length+= necessary_blocks_for_tkvs(string_tkv_list);
		}

	return total_length;
}

int create_partition(tkvs_per_partition_t* partition, int* blocks, int size_of_blocks,char* table_name) {
	t_list* string_tkv_list = list_map(partition->tkvs,convert_to_tkv); //hacer free TODO
	int size_of_all_tkvs_from_partition = size_of_all_tkvs(string_tkv_list);
	int blocks_amount = necessary_blocks_for_tkvs(string_tkv_list);
	//Si blocks amount da 0 significa que no tengo tkvs entonces le pongo un bloque vacio.
	if(blocks_amount == 0){
		int* blocks_for_the_table = array_take(blocks, size_of_blocks,1); //todo
		create_fs_archive(table_name,blocks_for_the_table,1,size_of_all_tkvs_from_partition,2,partition->partition);
		return size_of_blocks-1;

	}
	int* blocks_for_the_table = array_take(blocks, size_of_blocks,blocks_amount);
	create_fs_archive(table_name,blocks_for_the_table,blocks_amount,size_of_all_tkvs_from_partition,2,partition->partition);
	int block_size = fs_metadata->block_size;
	int block_index = 0;
	//Abro el .bin del primer bloque
	int block_to_open = blocks_for_the_table[block_index];
	FILE* block = open_block(block_to_open);

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
					readed_tkv->tkv-= what_the_pionter_moved;
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
				block_index++;
				int block_open = blocks_for_the_table[block_index];
				block = open_block(block_open);
				block_size = fs_metadata->block_size;
			}
		}
		//Si mi bloque se lleno o quedo con 1 solo carcter entonces lo cierro y paso al siguiente
		if (block_size <= 1) {
			fclose(block);
			block_index++;
			block = open_block(blocks_for_the_table[block_index]);
			block_size = fs_metadata->block_size;
		}

	}
	free(blocks_for_the_table);
	fclose(block);
	return size_of_blocks - blocks_amount;
}

void create_new_partitions(t_list* partition_tkvs,int* blocks, int size_of_blocks,char*  table_name){
	int blocks_length = size_of_blocks;
	for(int i=0; i<list_size(partition_tkvs);i++){
		tkvs_per_partition_t* partition = list_get(partition_tkvs,i);
		blocks_length = create_partition(partition,blocks,blocks_length,table_name);

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
				list_add(partition->tkvs, record);
				return;
			}
		//Si tiene la misma key pero menos timestamp no hago nada
		return;
		}

	}
	//Si no lo encuentra entronces lo agrega al final
	list_add(partition->tkvs, record);
}


void free_record(record_t* record){
	free(record->value);
	free(record);
}

t_list* create_partition_tkv_list(char* table_directory,table_metadata_t* table_metadata){
	t_list* partition_tkvs = list_create();
	for(int i=0; i<table_metadata->partitions;i++){
		char* partition_dir = create_partition_directory(table_directory, i+1);
		partition_t* partition = read_fs_archive( partition_dir);
		tkvs_per_partition_t* partition_for_the_list = malloc(sizeof(tkvs_per_partition_t));
		partition_for_the_list->partition = i+1;
		partition_for_the_list->tkvs = create_tkv_list(partition);
		list_add(partition_tkvs, partition_for_the_list);
		free(partition_dir);
		free(partition->blocks);
		free(partition);
	}
	return partition_tkvs;
}

t_list*  create_tkv_list(partition_t* partition) {
	tkv_t* key_found;
	int index = 0;
	int incomplete_tkv_size = 0;
	t_list* tkvs = list_create();

	for (int i = 0; i < partition->number_of_blocks; i++) {
		key_found = add_records_from_block(partition->blocks[i], index, incomplete_tkv_size, tkvs);
		index = 0;

		//Me fijo si encontro un tkv incompleto
		if (key_found->incomplete) {
			while (key_found->incomplete) {
				key_found->incomplete = false;
				char* continuation = read_first_tkv_in_block(partition->blocks[i + 1]);
				incomplete_tkv_size = strlen(continuation)+1;
				//Busco la siguiente parte y la concateno
				tkv_append(key_found,continuation);
				if (string_ends_with(key_found->tkv, "\n")) {
					char* substr = string_substring_until(key_found->tkv, strlen(key_found->tkv) - 1);
					strcpy(key_found->tkv, substr);
					key_found->incomplete = true;
					i++;
					free(substr);
				}

				free(continuation);
			}
			index = 1;
			//Una vez  reconstruido el tkv lo agrego
			record_t* record = convert_record(key_found->tkv);//Free? todo
			list_add(tkvs, record);

		}

		// limpiamos en cada iteracion
		//if (i < partition->number_of_blocks - 1) {
			free(key_found->tkv);
			free(key_found);
		//}
	}
	return tkvs;
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
	if (index == 1) {
		fread(readed_key, 1, incomplete_tkv_size, arch);
	}

	int i = 0;
	int pointer= 0;
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

		record_t* record = convert_record(readed_key); //Free? todo
		list_add(tkvs, record);
	}

	fclose(arch);
	free(readed_key);
	free(block_directory);
	return key_found_in_block;

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


void initialize_dump(){
	pthread_t config_thread;
	pthread_create(&config_thread, NULL, (void*) dump_all_tables, NULL);

}

void dump_all_tables(){
	pthread_detach(pthread_self());

	while(true){
		dump();
		usleep(g_config.dump_time * 1000);
	}
}

void dump(){

	if(mem_table->elements_count>0){
	//Saco cuantos bloques necesito para dumpear todas las tablas los cuales se calculan como:
	//tamaño de todos los tkvs/ tamaño de un bloque redondeado hacia arriba.
	int necessary_blocks = blocks_needed_for_memtable();
	//Creo un array de tantos bloques como los que necesito
	int* blocks= malloc(sizeof(int)*necessary_blocks);
	char* bitmap_dir = get_bitmap_directory();
	//Leo el bitmap
	sem_wait(bitmap_semaphore);
	t_bitarray* bitmap = read_bitmap(bitmap_dir);
	//Busco los bloques libres
	int free_blocks_amount = assign_free_blocks(bitmap, blocks, necessary_blocks);

	if(free_blocks_amount == necessary_blocks){


		for(int i=0; i<mem_table->elements_count;i++){
			table_t* table = list_get(mem_table,i);

			dump_table(table,blocks,free_blocks_amount);
			log_i("Dumpeados los datos de la tabla %s",table->name);
		}
		write_bitmap(bitmap, bitmap_dir);
	}else{
		log_w("No hay bloques suficientes como para dumpear todas las tablas de la memtable. Dumpeo cancelado");
	}
	sem_post(bitmap_semaphore);
	free_memtable();
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
	}
}

void create_fs_archive(char* table_name,int* blocks,int block_amount,int tkv_size,int archive_flag, int partition_number){
	char* initial_table_dir = get_table_directory();
	int tmp_number = 1;
	char* tmp_name = get_tmp_name(tmp_number);
	char* table_name_upper = to_uppercase(table_name);
	char* table_directory = create_new_directory(initial_table_dir,table_name_upper);

	while (exist_in_directory(tmp_name, table_directory)) {
		tmp_number++;
		free(tmp_name);
		tmp_name = get_tmp_name(tmp_number);
	}
	char* 	archive_dir = get_tmp_directory(table_directory, tmp_number);
	if(archive_flag == 0 ){
		free(	archive_dir);
		archive_dir = get_tmpc_directory(table_directory);

	}

	if(archive_flag == 2){
		free(archive_dir);
		char* partition_path = malloc(digits_in_a_number(partition_number) + strlen("/.bin") + 1);
		sprintf(partition_path, "/%d.bin", partition_number); //Esto transforma de int a string
		archive_dir = malloc(strlen(table_directory) + strlen(partition_path) + 1);
		strcpy(	archive_dir, table_directory);
		strcat(	archive_dir, partition_path);
		free(partition_path);

	}


	partition_t* partition = malloc(sizeof(partition_t));
	partition->number_of_blocks = block_amount;
	partition->size = tkv_size;
	partition->blocks = malloc(block_amount*sizeof(int));
	memcpy(partition->blocks,blocks,block_amount*sizeof(int));



	FILE* arch = fopen(archive_dir, "wb");
	fwrite(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks), arch);
	fwrite(partition->blocks, 1, sizeof(int) * partition->number_of_blocks, arch);
	fwrite(&partition->size, 1, sizeof(partition->size), arch);

	fclose(arch);
	free(partition->blocks);
	free(partition);
	free(initial_table_dir);
	free(archive_dir);
	free(tmp_name);
	free(table_name_upper);
	free(table_directory);
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

void dump_table(table_t* table, int* blocks, int size_of_blocks) {
	int size_of_all_tkvs_in_table = size_of_all_tkvs(table->tkvs);
	int blocks_amount = necessary_blocks_for_tkvs(table->tkvs);
	int* blocks_for_the_table = array_take(blocks, size_of_blocks,blocks_amount);
	create_fs_archive(table->name,blocks_for_the_table,blocks_amount,size_of_all_tkvs_in_table,1,0);
	int block_size = fs_metadata->block_size;
	int block_index = 0;
	//Abro el .bin del primer bloque
	int block_to_open = blocks_for_the_table[block_index];
	FILE* block = open_block(block_to_open);

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
					readed_tkv->tkv-= what_the_pionter_moved;
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
				block_index++;
				int block_open = blocks_for_the_table[block_index];
				block = open_block(block_open);
				block_size = fs_metadata->block_size;
			}
		}
		//Si mi bloque se lleno o quedo con 1 solo carcter entonces lo cierro y paso al siguiente
		if (block_size <= 1) {
			fclose(block);
			block_index++;
			block = open_block(blocks_for_the_table[block_index]);
			block_size = fs_metadata->block_size;
		}

	}
	free(blocks_for_the_table);
	fclose(block);
}

int necessary_blocks_for_tkvs(t_list* tkvs){
	int total_length = 0;

	for(int i =0;i<tkvs->elements_count;i++){
		tkv_t* tkv = list_get(tkvs,i);
		total_length += strlen(tkv->tkv);
		//Calculo cuantos /n tengo que agregar
		int extra_bits = strlen(tkv->tkv)/(fs_metadata->block_size-1);
		if((strlen(tkv->tkv)%fs_metadata->block_size) ==0){
			//si la division me da 0 entonces tengo un bit de mas asi que lo saco.
			extra_bits-=1;

		}
		total_length+=extra_bits;
		total_length = division_rounded_up(total_length,fs_metadata->block_size);
	}
	return total_length;
}

int size_of_all_tkvs(t_list* tkvs){
	int total_length = 0;

	for(int i =0;i<tkvs->elements_count;i++){
		tkv_t* tkv = list_get(tkvs,i);
		total_length += strlen(tkv->tkv);
		//Calculo cuantos /n tengo que agregar
		int extra_bits = strlen(tkv->tkv)/(fs_metadata->block_size-1);
		if((strlen(tkv->tkv)%fs_metadata->block_size) ==0){
			//si la division me da 0 entonces tengo un bit de mas asi que lo saco.
			extra_bits-=1;

		}
		total_length+=extra_bits;

	}
	return total_length;
}


int blocks_needed_for_memtable(){
	int total_length = 0;

	for(int i=0; i<mem_table->elements_count;i++){
		table_t* table = list_get(mem_table,i);
		total_length+= necessary_blocks_for_tkvs(table->tkvs);
	}
	return total_length;
}

int size_of_all_tkvs_from_table(){
	int total_length = 0;

	for (int i = 0; i < mem_table->elements_count; i++) {
		table_t* table = list_get(mem_table, i);
		total_length += size_of_all_tkvs(table->tkvs);
	}
	return total_length;
}




