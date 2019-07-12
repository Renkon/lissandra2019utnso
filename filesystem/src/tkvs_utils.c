#include "tkvs_utils.h"

int necessary_blocks_for_tkvs(t_list* tkvs){
	int total_length = 0;

	for(int i =0;i<tkvs->elements_count;i++){
		tkv_t* tkv = list_get(tkvs,i);
		int length_counter =0;
		length_counter += strlen(tkv->tkv);
		//Calculo cuantos /n tengo que agregar
		int extra_bits = strlen(tkv->tkv)/(fs_metadata->block_size-1);
		if((strlen(tkv->tkv)%fs_metadata->block_size) ==0){
			//si la division me da 0 entonces tengo un bit de mas asi que lo saco.
			extra_bits-=1;

		}
		length_counter+=extra_bits;
		total_length += division_rounded_up(length_counter,fs_metadata->block_size);
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

void free_tkvs_per_partition_list(t_list* list){
	for(int i=0; i<list_size(list);i++){
		tkvs_per_partition_t* partition = list_get(list,i);

		for(int d=0; d<list_size(partition->tkvs);d++){

			record_t* record = list_get(partition->tkvs,d);
			free_record(record);

		}
		list_clean(partition->tkvs);
		free(partition);

	}

}

void free_tkvs_per_partition(tkvs_per_partition_t* tkvs){
		list_destroy_and_destroy_elements(tkvs->tkvs,free_record);
		free(tkvs);
}

t_list*  create_tkv_list(partition_t* partition){
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
				int next_block= partition->blocks[i+1];
				char* continuation = read_first_tkv_in_block(next_block);
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
