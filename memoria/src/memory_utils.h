/*
 * memory_utils.h
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

#ifndef MEMORY_UTILS_H_
#define MEMORY_UTILS_H_

#include "memoria.h"
#include "memory_types.h"

#include <stdbool.h>
#include "utils/operation_types.h"
#include "generic_client.h"
#include "utils/operation_types.h"
#include "core/operations.h"
#include <commons/bitarray.h>
#include "utils/dates.h"
#include <limits.h>
#include "utils/numbers.h"

typedef enum {
	TIMESTAMP,
	KEY,
	VALUE,
	NULLEABLE
} memory_var_t;


int memory_insert(long long timestamp, int key, char* value);
bool memory_full();
char* main_memory_values(int index,memory_var_t type);
void modify_memory_by_index(int index,int key ,char* value);
bool order_by_timestamp(int first_i,int second_i);
void eliminate_page_instance_by_index(segment_t* segment, int index);
void journaling();
page_t* replace_algorithm(segment_t* segment,long long timestamp,int key, char* value);


#endif /* MEMORY_UTILS_H_ */

