#ifndef CREATE_CREATE_FILESYSTEM_H_
#define CREATE_CREATE_FILESYSTEM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../fs_lists.h"
#include "../lib/utils/operation_types.h"
#include "../lissandra/lissandra.h"
#include "../lissandra/table_state_utils.h"
#include <commons/bitarray.h>
#include "../directorys.h"
#include "../fs_lists.h"
#include "shared_types/shared_types.h"
#include "commons/string.h"

typedef struct {
	int number_of_blocks;
	int size;
	int* blocks;
} partition_t;

typedef struct {
	char* tkv;
	bool incomplete;
} tkv_t;


int create_table_folder(char* table_name);
void create_partitions(int partitions, char* table_name, int* blocks);
char* create_bin_name(int name);
void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name);
table_metadata_t* create_metadata(consistency_t consistency, int partitions,long compaction_time);
int assign_free_blocks(t_bitarray* bitmap, int* blocks, int partitions_amount);
int find_free_block(t_bitarray *bitmap);
record_t* search_key(char* table_directory, int key, char* table_name);
record_t* search_in_tmpc(char* table_directory, int key);
record_t* search_in_all_tmps(char* table_directory,int key);
record_t* search_in_partition(char* table_directory, int key);
record_t* create_tkv(insert_input_t* input);
void free_tkv(tkv_t* tkv);
bool value_exceeds_maximun_size(char* value);
void free_partitions(char* table_directory,t_bitarray* bitmap);
void free_blocks_of_fs_archive(char* archive_directory, t_bitarray* bitmap);
void free_block(int block);
void free_blocks_of_all_tmps(char* table_directory, t_bitarray* bitmap);
void free_table(table_t* table);
record_t* convert_record(char* tkv_string);
void free_record(record_t* record);
void add_table_to_table_state_list(char* table_name);
table_state_t* find_in_table_state_list(char* table_name);
void is_blocked_wait(char* table_name);
void is_blocked_wait(char* table_name);
void is_blocked_post(char* table_name);
void live_status_wait(char* table_name);
void live_status_post(char* table_name);
int* get_live_status(char* table_name);
table_state_t* find_in_table_state_list_with_thread(pthread_t thread);
record_t* search_key_in_memtable(int key, char* table_name);
table_t* search_table_in_memtable(char* table_name);

#endif /* CREATE_CREATE_UTILS_H_ */
