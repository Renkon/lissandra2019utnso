#include "response.h"

response_t* generate_response_object() {
	response_t* response = malloc(sizeof(response_t));
	response->id = generate_id();
	response->result = NULL;
	response->semaphore = malloc(sizeof(sem_t));

	sem_init(response->semaphore, 0, 0);

	return response;
}

void wait_for_response(response_t* response) {
	sem_wait(response->semaphore);
}

void set_response(response_t* response, void* result) {
	response->result = result;

	// si no se bloqueo no pasa nada, hace el post igual.
	sem_post(response->semaphore);
}

void destroy_response(response_t* response, socket_operation_t operation) {
	sem_destroy(response->semaphore);
	free(response->semaphore);

	// El cleanup del result varia segun el tipo
	switch (operation) {
		case SELECT_OUT:
			free(((record_t*) response->result)->table_name);
			free(((record_t*) response->result)->value);
			free(response->result);
		break;
		case DESCRIBE_OUT:
			for (int i = 0; i < list_size((t_list*) response->result); i++) {
				free(((table_metadata_t*) list_get((t_list*) response->result, i))->table_name);
				free(list_get((t_list*) response->result, i));
			}
			list_destroy((t_list*) response->result);
		break;
		default:
			free(response->result);
		break;
	}

	response->result = NULL;

	remove_id(response->id);
	free(response);
}

void setup_response_id_generator() {
	g_response_ids = list_create();
}

int generate_id() {
	int* new_id = malloc(sizeof(int));
	int size = list_size(g_response_ids);

	for (int i = 0; i < size; i++) {
		if (*((int*) list_get(g_response_ids, i)) != i) {
			*new_id = i;
			list_add_in_index(g_response_ids, i, new_id);
			return i;
		}
	}

	*new_id = size;
	list_add(g_response_ids, new_id);

	return size;
}

void remove_id(int id) {
	int* id_ptr;
	int size = list_size(g_response_ids);

	for (int i = 0; i < size; i++) {
		if ((*(id_ptr = list_get(g_response_ids, i))) == id) {
			list_remove(g_response_ids, i);
			free(id_ptr);
			return;
		}
	}
}
