

#ifndef CREATE_CREATE_UTILS_H_
#define CREATE_CREATE_UTILS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../lissandra/filesystem.h"


void create_table_folder(char* table_name);
char *create_table_path(char* table_name);


#endif /* CREATE_CREATE_UTILS_H_ */
