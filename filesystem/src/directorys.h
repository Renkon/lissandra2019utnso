#ifndef DIRECTORYS_H_
#define DIRECTORYS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include "utils/numbers.h"
#include "filesystem/filesystem.h"
#include <commons/collections/list.h>
#include "fs_lists.h"

char* get_tmpc_name;


char* get_table_directory();
char* get_block_directory();
char* get_bitmap_directory();
char* get_tmpc_directory(char* table_directory);
char* get_tmp_name(int tmp_number);
char* get_tmp_directory (char*  table_directory,int tmp_number);
char* create_metadata_directory(char* table_directory);
char* create_partition_directory(char* table_directory, int partition_number);
char* create_new_directory(char* old_directory, char* directory_end);
char* create_partition_name(int partition_number);
char* create_block_directory (int block_number);
bool exist_in_directory(char* archive, char* directory);
t_list* get_tables_list();
int remove_directory(char *path);
metadata_t* read_fs_metadata();
FILE* open_block(int block);
void write_tkv(char* tkv,FILE* block);

#endif /* DIRECTORYS_H_ */
