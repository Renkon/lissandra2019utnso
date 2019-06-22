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
			dump_table(table,blocks);
		}
	}else{
		log_w("No hay bloques suficientes como para dumpear todas las tablas de la memtable. Dumpeo cancelado");
	}
	//LIberar la memtable TODO
	free(bitmap->bitarray);
	free(bitmap);
	free(bitmap_dir);

}

void dump_table(table_t* table,int* blocks){
	int size_of_all_tkvs_from_table = tkv_total_length( table->tkvs);

	int blocks_amount = division_rounded_up(size_of_all_tkvs_from_table, fs_metadata->block_size);
	//Hacer una funcion que saque los primeros n bloques del array y los devuelva (pero que tenga efecto)
	for (int i = 0; i < table->tkvs->elements_count; i++) {


	}

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
