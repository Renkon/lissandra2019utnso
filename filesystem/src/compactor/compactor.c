#include "compactor.h"

void dump(){

	for(int i=0; i<mem_table->elements_count;i++){
		table_t* table = list_get(mem_table,i);
		dump_table(table);

	}

}

void dump_table(table_t* table){

	for(int i=0; i<table->tkvs->elements_count;i++){

			table->tkvs;
			}


}

int search_free_blocks_for_table(t_list* tkvs){



}


int tkv_total_length(t_list* tkvs){
	int total_length = 0;

	for(int i =0;i<tkvs->elements_count;i++){

		tkv_t* tkv = list_get(tkvs,i);
		total_length += strlen(tkv->tkv);
		free(tkv);
	}

	return total_length;
}
