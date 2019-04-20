#include "pid_utils.h"

void setup_pid_generator() {
	g_process_ids = list_create();
}

int generate_pid() {
	int* new_pid = malloc(sizeof(int));
	int size = list_size(g_process_ids);

	for (int i = 0; i < size; i++) {
		if (*((int*) list_get(g_process_ids, i)) != i) {
			*new_pid = i;
			list_add_in_index(g_process_ids, i, new_pid);
			return i;
		}
	}

	*new_pid = size;
	list_add(g_process_ids, new_pid);

	return size;
}

void remove_pid(int id) {
	int* pid_ptr;

	for (int i = 0; i < list_size(g_process_ids); i++) {
		if ((*(pid_ptr = list_get(g_process_ids, i))) == id) {
			list_remove(g_process_ids, i);
			free(pid_ptr);
			return;
		}
	}
}
