#ifndef CREATE_CREATE_FILESYSTEM_H_
#define CREATE_CREATE_FILESYSTEM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "../directorys.h"
#include "../lissandra/lissandra.h"

typedef struct table_metadata_t {
	consistency_t consistency;
	int partitions;
	long compaction_time;

} Table_metadata;

typedef struct partition_t {
	int size;
	int* blocks;
	int number_of_blocks;

} Partition;


typedef struct key_t {
	long long timestamp;
	int key;
	char* value;

} Key;

int create_table_folder(char* table_name);
void create_partitions(int partitions, char* table_name, int* blocks);
char* create_bin_name(int name);
void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name);
Table_metadata* create_metadata(consistency_t consistency, int partitions,long compaction_time);
int assign_free_blocks(t_bitarray* bitmap, int* blocks, int* partitions_amount);
int find_free_block(t_bitarray *bitmap);

#endif /* CREATE_CREATE_UTILS_H_ */
