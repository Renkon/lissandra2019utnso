#include "memories.h"

void init_memory_list() {
	g_memories_added_sc = list_create();
	g_memories_added_shc = list_create();
	g_memories_added_ec = list_create();
	ec_next = -1;
}

memory_t* get_any_memory() {
	if (list_size(g_memories) == 0) {
		log_e("No hay memorias en la tabla de memorias");
		return NULL;
	}

	t_list* alive_memories = list_filter(g_memories, is_memory_alive);
	if (list_size(alive_memories) == 0) {
		log_e("No hay memorias vivas para responder la peticion");
		return NULL;
	}

	int random_memory = rnd(0, list_size(alive_memories) - 1);
	memory_t* selected = (memory_t*) list_get(alive_memories, random_memory);
	list_destroy(alive_memories);

	log_t("Se asigno la memoria %i para la operacion", selected->id);
	return selected;
}

memory_t* get_any_sc_memory() {
	if (list_size(g_memories_added_sc) == 0) {
		log_e("No hay memorias para el criterio STRONG CONSISTENCY");
		return NULL;
	}

	memory_t* selected = (memory_t*) list_get(g_memories_added_sc, 0);

	log_t("Se asigno la memoria %i para la operacion", selected->id);
	return selected;
}

memory_t* get_any_shc_memory(uint16_t key) {
	if (list_size(g_memories_added_shc) == 0) {
		log_e("No hay memorias para el criterio STRONG HASH CONSISTENCY");
		return NULL;
	}

	int assigned_memory = hash((int) key, list_size(g_memories_added_shc));

	memory_t* selected = (memory_t*) list_get(g_memories_added_shc, assigned_memory);

	log_t("Se asigno la memoria %i para la operacion", selected->id);
	return selected;
}

memory_t* get_any_ec_memory() {
	if (list_size(g_memories_added_ec) == 0) {
		log_e("No hay memorias para el criterio EVENTUAL CONSISTENCY");
		return NULL;
	}

	if (ec_next >= list_size(g_memories_added_ec) - 1)
		ec_next = 0;
	else
		ec_next++;

	memory_t* selected = (memory_t*) list_get(g_memories_added_ec, ec_next);

	log_t("Se asigno la memoria %i para la operacion", selected->id);
	return selected;
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

	// TODO: forzar journaling en memorias del SHC.
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

	// TODO: forzar journaling aca para las memorias de SHC.
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

void journaling(bool only_shc, void (*callback)(void*, response_t*), pcb_t* pcb) {
	// TODO: wait for them to finish.
	if (only_shc) {
		for (int i = 0; i < list_size(g_memories_added_shc); i++) {
			memory_t* memory = (memory_t*) list_get(g_memories_added_shc, i);
			elements_network_t element_info = elements_journal_in_info(NULL);

			// TODO: ver como carajo hacer para esperar a todas las memorias que hagan journal
			// un semaforo que inicialize en 0 mas un wait por cada journal? local_journal_callback ejecutaria el callback despues de qu etermine todito
			//do_simple_request(KERNEL, memory->ip, memory->port, JOURNAL_IN, NULL,
			//		element_info.elements, element_info.elements_size, local_journal_callback, true, NULL, pcb);
		}
	} else {
		t_list* memories_with_consistency = get_list_with_a_consistency();
		for (int i = 0; i < list_size(memories_with_consistency); i++) {
			memory_t* memory = (memory_t*) list_get(g_memories_added_shc, i);
			elements_network_t element_info = elements_journal_in_info(NULL);

			// TODO: ver como carajo hacer para esperar a todas las memorias que hagan journal
			// un semaforo que inicialize en 0 mas un wait por cada journal? local_journal_callback ejecutaria el callback despues de qu etermine todito
			//do_simple_request(KERNEL, memory->ip, memory->port, JOURNAL_IN, NULL,
			//		element_info.elements, element_info.elements_size, local_journal_callback, true, NULL, pcb);
		}
		list_destroy(memories_with_consistency);
	}
}

t_list* get_list_with_a_consistency() {
	t_list* mems = list_create();

	void _add_to_mems_if_not_found(t_list* memlist) {
		for (int i = 0; i < list_size(memlist); i++) {
			memory_t* mem = (memory_t*) list_get(memlist, i);
			bool found_already = false;

			for (int j = 0; j < list_size(mems); j++) {
				memory_t* mem2 = (memory_t*) list_get(mems, j);

				if (mem2 == mem) {
					found_already = true;
					break;
				}
			}

			if (!found_already) {
				log_t("Agrego %i a la lista", mem->id);
				list_add(mems, mem);
			}
		}
	}

	_add_to_mems_if_not_found(g_memories_added_sc);
	_add_to_mems_if_not_found(g_memories_added_shc);
	_add_to_mems_if_not_found(g_memories_added_ec);

	return mems;
}

