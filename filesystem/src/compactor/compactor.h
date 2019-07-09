#ifndef COMPACTOR_COMPACTOR_H_
#define COMPACTOR_COMPACTOR_H_

#include <stdio.h>
#include "../lissandra/lissandra.h"
#include <commons/collections/list.h>
#include "../filesystem/filesystem.h"
#include "utils/numbers.h"
#include "utils/int_arrays.h"

typedef struct {
	int	partition;
	t_list*	tkvs;

}tkvs_per_partition_t;

void dump_all_tables();
void dump();
void free_memtable();
void dump_table(table_t* table, int* blocks, int size_of_blocks);
int search_free_blocks_for_table(t_list* tkvs);
int tkv_total_length(t_list* tkvs);
int length_of_all_tkvs_in_memtable();
void compaction(char* table_name);
partition_t* get_all_blocks_from_all_tmps (char* table_name);
void create_fs_archive(char* table_name,int* blocks,int block_amount,int tkv_size,int archive_flag, int partition_number);
tkv_t* add_records_from_block(int block, int index, int incomplete_tkv_size,t_list* tkvs);
record_t* convert_record(char* tkv_string);
t_list*  create_tkv_list(partition_t* partition);
t_list* create_partition_tkv_list(char* table_directory,table_metadata_t* table_metadata);
void get_tkvs_to_insert(t_list* tmpc_tkvs, t_list* partition_tkvs);
void add_record_to_partition_list (record_t* record,tkvs_per_partition_t* partition);
void free_record(record_t* record);
void create_new_partitions(t_list* partition_tkvs,int* blocks, int size_of_blocks,char*  table_name);
int length_of_all_tkvs_in_partitions_to_add(t_list* partition_tkvs);
int create_partition(tkvs_per_partition_t* partition, int* blocks, int size_of_blocks,char* table_name);
tkv_t* convert_to_tkv(record_t* record);
#endif /* COMPACTOR_COMPACTOR_H_ */
