#include "compactor.h"

void compaction(char* table_name){
	partition_t* tmpc =get_all_blocks_from_all_tmps(table_name);
	//Creo el tmpc
	create_tmp(table_name,tmpc->blocks,tmpc->number_of_blocks,tmpc->size,0);
	//Matar todos los tmps todo;
	t_list* tmpc_tkvs = create_tkv_list(tmpc);
	free(tmpc->blocks);
	free(tmpc);
}


t_list*  create_tkv_list(partition_t* partition) {
	record_t* key_found_in_block = malloc(sizeof(record_t));
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"
	key_found_in_block->timestamp = -1;
	tkv_t* key_found;
	tkv_t* correct_key_found = malloc(sizeof(tkv_t));
	correct_key_found ->tkv = malloc(strlen("-1;-1;-1")+1);
	strcpy(correct_key_found->tkv,"-1;-1;-1");
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
			record_t* record = convert_record(key_found->tkv); //Free? todo
			list_add(tkvs, record);

		}

		// limpiamos en cada iteracion
		if (i < partition->number_of_blocks - 1) {
			free(key_found->tkv);
			free(key_found);
		}
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
	int necessary_blocks = division_rounded_up(length_of_all_tkvs_in_memtable(), fs_metadata->block_size);
	printf("%d",necessary_blocks);
	//Creo un array de tantos bloques como los que necesito
	int blocks[necessary_blocks];
	char* bitmap_dir = get_bitmap_directory();
	//Leo el bitmap
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

	free_memtable();
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);
	}
}

void create_tmp(char* table_name,int* blocks,int block_amount,int tkv_size,int tmp_flag){
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
	char* tmp_dir = get_tmp_directory(table_directory, tmp_number);
	if(tmp_flag == 0 ){
		free(tmp_dir);
		tmp_dir = get_tmpc_directory(table_directory);

	}
	partition_t* partition = malloc(sizeof(partition_t));
	partition->number_of_blocks = block_amount;
	partition->size = tkv_size;
	partition->blocks = malloc(block_amount*sizeof(int));
	memcpy(partition->blocks,blocks,block_amount*sizeof(int));



	FILE* arch = fopen(	tmp_dir, "wb");
	fwrite(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks), arch);
	fwrite(partition->blocks, 1, sizeof(int) * partition->number_of_blocks, arch);
	fwrite(&partition->size, 1, sizeof(partition->size), arch);

	fclose(arch);
	free(partition->blocks);
	free(partition);
	free(initial_table_dir);
	free(tmp_dir);
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
	int size_of_all_tkvs_from_table = tkv_total_length(table->tkvs);
	int blocks_amount = division_rounded_up(size_of_all_tkvs_from_table,fs_metadata->block_size);
	//Hacer una funcion que saque los primeros n bloques del array y los devuelva (pero que tenga efecto)
	int* blocks_for_the_table = array_take(blocks, size_of_blocks,blocks_amount);
	//CREAR TMP
	create_tmp(table->name,blocks_for_the_table,blocks_amount,size_of_all_tkvs_from_table,1);
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

int tkv_total_length(t_list* tkvs){
	int total_length = 0;

	for(int i =0;i<tkvs->elements_count;i++){
		tkv_t* tkv = list_get(tkvs,i);
		total_length += strlen(tkv->tkv);
		//Calculo cuantos /n tengo que agregar
		int extra_bits = strlen(tkv->tkv)/fs_metadata->block_size;
		if((strlen(tkv->tkv)%fs_metadata->block_size) ==0){
			//si la division me da 0 entonces tengo un bit de mas asi que lo saco.
			extra_bits-=1;

		}
		total_length+=extra_bits;
	}
	return total_length;
}

int length_of_all_tkvs_in_memtable(){
	int total_length = 0;

	for(int i=0; i<mem_table->elements_count;i++){
		table_t* table = list_get(mem_table,i);
		total_length+= tkv_total_length(table->tkvs);
	}
	return total_length;
}

