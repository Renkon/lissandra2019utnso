#include <stdbool.h>
#include <stddef.h>
#include <semaphore.h>
#include <stdlib.h>
#include "commons/collections/list.h"

#ifndef UTILS_RESPONSE_H_
#define UTILS_RESPONSE_H_

typedef struct {
	int id;
	sem_t* semaphore;
	void* result;
} response_t;

t_list* g_response_ids;

response_t* generate_response_object();
void wait_for_response(response_t* response);
void set_response(response_t* response, void* result);
void destroy_response(response_t* response);
void setup_response_id_generator();
int generate_id();
void remove_id(int id);

#endif /* UTILS_RESPONSE_H_ */
