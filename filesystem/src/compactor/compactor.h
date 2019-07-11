#ifndef COMPACTOR_COMPACTOR_H_
#define COMPACTOR_COMPACTOR_H_

#include <stdio.h>
#include "../lissandra/lissandra.h"
#include <commons/collections/list.h>
#include "../filesystem/filesystem.h"
#include "utils/numbers.h"
#include "utils/int_arrays.h"
#include "../tkvs_utils.h"

void dump_all_tables();
void dump();
void free_memtable();
void dump_table(table_t* table, int* blocks, int size_of_blocks);
int search_free_blocks_for_table(t_list* tkvs);
int tkv_total_length(t_list* tkvs);
int length_of_all_tkvs_in_memtable();
void compaction(char* table_name);
partition_t* get_all_blocks_from_all_tmps (char* table_name);
tkv_t* add_records_from_block(int block, int index, int incomplete_tkv_size,t_list* tkvs);
void get_tkvs_to_insert(t_list* tmpc_tkvs, t_list* partition_tkvs);
void add_record_to_partition_list (record_t* record,tkvs_per_partition_t* partition);
void create_new_partitions(t_list* partition_tkvs,t_list* blocks, int size_of_blocks,char*  table_name);
tkv_t* convert_to_tkv(record_t* record);
int add_blocks_for_partitions_without_tkvs(t_list* partition_tkvs);
void destroy_all_tmps(char* table_directory);
int*from_list_to_array(t_list* list);
t_list* from_array_to_list (int* array, int size);
int create_partition(tkvs_per_partition_t* partition, t_list* blocks, int size_of_blocks,char* table_name);

#endif /* COMPACTOR_COMPACTOR_H_ */
