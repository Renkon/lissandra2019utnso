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
#include "utils/response.h"
#include <commons/bitarray.h>
#include <pthread.h>
#include <semaphore.h>
#include "utils/dates.h"
#include <unistd.h>
#include <limits.h>
#include "utils/numbers.h"

typedef enum {
	TIMESTAMP,
	KEY,
	VALUE,
	NULLEABLE
} memory_var_t;

typedef enum {
	J_SELECT,
	J_INSERT
} journal_invocation_t;

typedef struct {
	int key;
	char* table_name;
	char* value;
	long long timestamp;
	bool modified;
} journal_register_t;

sem_t g_mem_op_semaphore;

int memory_insert(long long timestamp, int key, char* value);
bool memory_full();
char* main_memory_values(int index,memory_var_t type);
void modify_memory_by_index(int index,int key ,char* value);
bool order_by_timestamp(int first_i,int second_i);
void eliminate_page_instance_by_index(int index);
void journaling(response_t* response);
page_t* replace_algorithm(response_t* response, long long timestamp,int key, char* value, journal_invocation_t invocation, char* table_name);

/* Ir a buscar el value */
void init_value_checker();
void check_value();
void get_value_from_filesystem();
void get_value_callback(void* result, response_t* response);

void init_main_memory();

void sem_post_neg(sem_t* sem);

#endif /* MEMORY_UTILS_H_ */

