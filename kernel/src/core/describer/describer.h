#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include "commons/collections/list.h"
#include "shared_types/shared_types.h"
#include "../../config.h"
#include "../gossiping/gossiping.h"
#include "commons/string.h"
#include "generic_client.h"
#include "utils/network_types.h"
#include "utils/operation_types.h"
#include "utils/response.h"

#ifndef CORE_DESCRIBER_DESCRIBER_H_
#define CORE_DESCRIBER_DESCRIBER_H_

typedef struct {
	t_list* metadata_list;
	bool single;
	bool upsert;
} update_metadata_args_t;

t_list* g_table_metadata;
sem_t g_mutex_user_describe;

void init_describer();
void update_describer_continuously();
void update_descriptions();
void update_descriptions_callback(void* result, response_t* response);
void on_describe_update_triggered(void* untyped_args);
void on_post_describe(t_list* metadata_list, bool single, bool upsert);
void on_post_create(create_input_t* input);
void on_post_drop(drop_input_t* input);
void display_metadata();

bool table_exists_in_metadata(char* table_name);

void free_auto_describe_input(void* input);

#endif /* CORE_DESCRIBER_DESCRIBER_H_ */
