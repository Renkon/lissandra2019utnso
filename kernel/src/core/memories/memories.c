#include "memories.h"

void init_memory_list() {
	g_memories_added_sc = list_create();
	g_memories_added_shc = list_create();
	g_memories_added_ec = list_create();
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
