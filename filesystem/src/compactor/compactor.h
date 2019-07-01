#ifndef COMPACTOR_COMPACTOR_H_
#define COMPACTOR_COMPACTOR_H_

#include <stdio.h>
#include "../lissandra/lissandra.h"
#include <commons/collections/list.h>
#include "../filesystem/filesystem.h"
#include "utils/numbers.h"
#include "utils/int_arrays.h"

void dump_all_tables();
void dump();
void free_memtable();
void dump_table(table_t* table, int* blocks, int size_of_blocks);
int search_free_blocks_for_table(t_list* tkvs);
int tkv_total_length(t_list* tkvs);
int length_of_all_tkvs_in_memtable();
void create_table_tmp(char* table_name,int* blocks,int block_amount,int tkv_size);
#endif /* COMPACTOR_COMPACTOR_H_ */
