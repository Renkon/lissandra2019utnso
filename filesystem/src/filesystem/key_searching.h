
#ifndef FILESYSTEM_KEY_SEARCHING_H_
#define FILESYSTEM_KEY_SEARCHING_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "filesystem.h"
#include <limits.h>
#include "utils/dates.h"
#include "string.h"

table_metadata_t* read_table_metadata(char* table_directory);
partition_t* read_fs_archive(char* partition_string);
record_t* search_key_in_partition(char* partition_path, int key);
int tkv_size();
char* read_first_tkv_in_block(int block);
tkv_t* search_key_in_block(int block, char* key, int index, int incomplete_tkv_size);
void convert_to_record(record_t* record,tkv_t* tkv);
record_t* copy_key(record_t* key_to_copy);
record_t* key_with_greater_timestamp(record_t* key_1, record_t* key_2);
void tkv_append(tkv_t* tkv,char* end);

#endif /* FILESYSTEM_KEY_SEARCHING_H_ */
