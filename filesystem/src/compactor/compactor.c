#include "compactor.h"

void dump(){
	//Saco cuantos bloques necesito para dumpear todas las tablas los cuales se calculan como:
	//tamaño de todos los tkvs/ tamaño de un bloque redondeado hacia arriba.
	int necessary_blocks = division_rounded_up(length_of_all_tkvs_in_memtable(), fs_metadata->block_size);
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

void create_table_tmp(char* table_name,int* blocks,int block_amount,int tkv_size){
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
	create_table_tmp(table->name,blocks_for_the_table,blocks_amount,size_of_all_tkvs_from_table);
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
			while (true) {
				//Me fijo si el string entra en el bloque
				int free_space_in_block2 = block_size - strlen(readed_tkv->tkv);
				if (free_space_in_block2 > 0) {
					//SI entra lo guardo directamente y corto
					write_tkv(readed_tkv->tkv, block);
					block_size-=strlen(readed_tkv->tkv);
					break;
				}
				//SI no entra entonces guardo lo que si entra con un \n al final y vuelvo a intentar con un nuevo bloque
				char* tkv_that_enters = string_substring_until(readed_tkv->tkv,block_size - 1);
				readed_tkv->tkv += block_size - 1;
				string_append(tkv_that_enters, "\n");
				write_tkv(tkv_that_enters,block);
				block_index++;
				fclose(block);
				block = open_block(blocks[block_index]);
				block_size = fs_metadata->block_size;
			}
		}
		//Si mi bloque se lleno o quedo con 1 solo carcter entonces lo cierro y paso al siguiente
		if (block_size <= 1) {
			block_index++;
			fclose(block);
			block = open_block(blocks[block_index]);
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
