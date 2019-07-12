#ifndef FILESYSTEM_TABLE_UTILS_H_
#define FILESYSTEM_TABLE_UTILS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "fs_lists.h"
#include <commons/collections/list.h>
#include "shared_types/shared_types.h"



table_t* create_table(char* name);

#endif /* FILESYSTEM_TABLE_UTILS_H_ */
