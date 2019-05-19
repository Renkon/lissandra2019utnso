
#ifndef FILESYSTEM_KEY_SEARCHING_H_
#define FILESYSTEM_KEY_SEARCHING_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "filesystem.h"

table_metadata_t* read_table_metadata(char* table_directory);
partition_t* read_fs_archive(char* partition_string);
record_t* search_key_in_partition(char* partition_path, int key);
record_t* search_key_in_block(int block,int key);
record_t* copy_key(record_t* key_to_copy);
record_t* key_with_greater_timestamp(record_t* key_1, record_t* key_2);

#endif /* FILESYSTEM_KEY_SEARCHING_H_ */
