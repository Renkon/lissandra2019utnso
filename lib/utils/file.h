#ifndef UTILS_FILE_H_
#define UTILS_FILE_H_

#include <stdio.h>
#include <string.h>
#include "../generic_logger.h"
#include "commons/collections/list.h"

t_list* get_file_lines(char* file);

#endif /* UTILS_FILE_H_ */
