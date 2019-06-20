#include "response.h"

response_t* generate_response_object() {
	response_t* response = malloc(sizeof(response_t));
	response->id = generate_id();
	response->result = NULL;
	response->with_result = false;
	response->semaphore = malloc(sizeof(sem_t));

	sem_init(response->semaphore, 0, 1);

	return response;
}

void wait_for_response(response_t* response) {
	sem_wait(response->semaphore);
}

void set_response(response_t* response, bool with_result, void* result) {
	response->with_result = with_result;

	if (with_result)
		response->result = result;

	// si no se bloqueo no pasa nada, hace el post igual.
	sem_post(response->semaphore);
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
