

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include "lissandra/lissandra.h"


char *get_table_directory();
char *get_block_directory();
char* create_new_directory(char* old_directory, char* directory_end);
int exist_in_directory(char* archive, char* directory);


#endif /* UTILS_H_ */
