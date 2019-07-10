#include "memories.h"

void init_memory_list() {
	g_memories_added_sc = list_create();
	g_memories_added_shc = list_create();
	g_memories_added_ec = list_create();
	ec_next = 0;
}

memory_t* get_any_memory() {
	if (list_size(g_memories) == 0)
		return NULL;

	int random_memory = rnd(0, list_size(g_memories));

	return (memory_t*) list_get(g_memories, random_memory);
}

memory_t* get_any_sc_memory() {
	if (list_size(g_memories_added_sc) == 0)
		return NULL;

	return (memory_t*) list_get(g_memories_added_sc, 0);
}

memory_t* get_any_shc_memory(char* table_name, uint16_t key) {
	int hash_value = hash(table_name, (int) key);
	int assigned_memory = hash_value % list_size(g_memories_added_shc);

	return (memory_t*) list_get(g_memories_added_shc, assigned_memory);
}

memory_t* get_any_ec_memory() {
	if (list_size(g_memories_added_ec) == 0)
		return NULL;

	if (ec_next >= list_size(g_memories_added_ec) - 1)
		ec_next = 0;
	else
		ec_next++;

	return (memory_t*) list_get(g_memories_added_ec, ec_next);
}

void add_sc_memory(int id) {
	memory_t* memory = get_memory_by_id(g_memories, id);

	if (memory == NULL) {
		log_e("No se pudo asignar la memoria %i al criterio SC. Esta memoria no es reconocida por el Kernel", id);
		return;
	}

	if (get_memory_by_id(g_memories_added_sc, id) != NULL) {
		log_w("La memoria %i ya se encuentra asignada al criterio STRONG CONSISTENCY", id);
		return;
	}

	if (list_size(g_memories_added_sc) > 0) {
		log_w("Ya hay una memoria asignada al criterio STRONG CONSISTENCY");
		return;
	}

	list_add(g_memories_added_sc, memory);
	log_i("Se agrego la memoria %i al criterio STRONG CONSISTENCY", id);
}

void add_shc_memory(int id) {
	memory_t* memory = get_memory_by_id(g_memories, id);

	if (memory == NULL) {
		log_e("No se pudo asignar la memoria %i al criterio SHC. Esta memoria no es reconocida por el Kernel", id);
		return;
	}

	if (get_memory_by_id(g_memories_added_shc, id) != NULL) {
		log_w("La memoria %i ya se encuentra asignada al criterio STRONG HASH CONSISTENCY", id);
		return;
	}

	list_add(g_memories_added_shc, memory);
	log_i("Se agrego la memoria %i al criterio STRONG HASH CONSISTENCY", id);
}

void add_ec_memory(int id) {
	memory_t* memory = get_memory_by_id(g_memories, id);

	if (memory == NULL) {
		log_e("No se pudo asignar la memoria %i al criterio EC. Esta memoria no es reconocida por el Kernel", id);
		return;
	}

	if (get_memory_by_id(g_memories_added_ec, id) != NULL) {
		log_w("La memoria %i ya se encuentra asignada al criterio EVENTUAL CONSISTENCY", id);
		return;
	}

	list_add(g_memories_added_ec, memory);
	log_i("Se agrego la memoria %i al criterio EVENTUAL CONSISTENCY", id);
}

void remove_memory(int id) {
	remove_memory_from_consistency(g_memories_added_sc, id);
	remove_memory_from_consistency(g_memories_added_shc, id);
	remove_memory_from_consistency(g_memories_added_ec, id);
	log_t("Se elimino la memoria %i de todas las cosistencias (si es que estaba)");
}

memory_t* get_memory_by_id(t_list* mem_list, int id) {
	for (int i = 0; i < list_size(mem_list); i++)
	{
		memory_t* mem = (memory_t*) list_get(mem_list, i);

		if (mem->id == id)
			return mem;
	}

	return NULL;
}

void remove_memory_from_consistency(t_list* mems, int id) {
	for (int i = 0; i < list_size(mems); i++) {
		memory_t* mem = (memory_t*) list_get(mems, i);
		if (mem->id == id) {
			list_remove(mems, i);
			break;
		}
	}
}

memory_t* get_random_memory(bool alive) {
	t_list* alive_memories = NULL;
	t_list* gossipable_memories;

	if (alive) {
		alive_memories = list_filter(g_memories, is_memory_alive);
		if (list_size(alive_memories) == 0) {
			list_destroy(alive_memories);
			return NULL;
		} else {
			gossipable_memories = alive_memories;
		}
	} else {
		gossipable_memories = g_memories;
	}

	int random_memory = rnd(0, list_size(gossipable_memories) - 1);
	memory_t* memory = (memory_t*) list_get(gossipable_memories, random_memory);

	if (alive_memories != NULL)
		list_destroy(alive_memories);

	return memory;
}

bool is_memory_alive(void* memory) {
	return ((memory_t*) memory)->alive;
}

