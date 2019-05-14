
#ifndef FILESYSTEM_KEY_SEARCHING_H_
#define FILESYSTEM_KEY_SEARCHING_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "filesystem.h"

Table_metadata* read_table_metadata(char* table_directory);
Partition* read_partition(char* partition_string);
Key* search_key_in_partition(char* partition_path, int key);
Key* search_key_in_block(int block,int key);
#endif /* FILESYSTEM_KEY_SEARCHING_H_ */
