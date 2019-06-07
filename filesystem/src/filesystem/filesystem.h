#ifndef CREATE_CREATE_FILESYSTEM_H_
#define CREATE_CREATE_FILESYSTEM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>


#include "../directorys.h"
#include "../lissandra/lissandra.h"
#include "../fs_lists.h"


typedef struct  {
	consistency_t consistency;
	int partitions;
	long compaction_time;
} table_metadata_t;

typedef struct {
	int number_of_blocks;
	int size;
	int* blocks;
} partition_t;


typedef struct {
	long long timestamp;
	int key;
	char* value;
	int value_length; //Sacar
} record_t;

typedef struct {
	char* tkv;
	bool incomplete;
} tkv_t;

int create_table_folder(char* table_name);
void create_partitions(int partitions, char* table_name, int* blocks);
char* create_bin_name(int name);
void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name);
table_metadata_t* create_metadata(consistency_t consistency, int partitions,long compaction_time);
int assign_free_blocks(t_bitarray* bitmap, int* blocks, int* partitions_amount);
int find_free_block(t_bitarray *bitmap);
record_t* search_key (char* table_directory, int key);
record_t* search_in_tmpc(char* table_directory, int key);
record_t* search_in_all_tmps(char* table_directory,int key);
record_t* search_in_partition(char* table_directory, int key);
record_t* create_record(insert_input_t* input);
bool value_exceeds_maximun_size(char* value);
void free_partitions(char* table_directory,t_bitarray* bitmap);
void free_blocks_of_fs_archive(char* archive_directory, t_bitarray* bitmap);
void free_block(int block);
void free_blocks_of_all_tmps(char* table_directory, t_bitarray* bitmap);

#endif /* CREATE_CREATE_UTILS_H_ */
