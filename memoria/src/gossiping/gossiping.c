#include "gossiping.h"

void init_gossiping() {
	pthread_t gossiping_thread;

	g_memories = list_create();

	memory_t* me = malloc(sizeof(memory_t));
	me->alive = true;
	me->id = g_config.memory_number;
	me->ip = strdup(g_config.ip);
	me->port = g_config.port;
	me->timestamp = get_timestamp();

	list_add(g_memories, me);

	sem_init(&g_mutex_memories, 0, 1);

	if (pthread_create(&gossiping_thread, NULL, (void*) gossip_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de gossiping");
	}
}

void gossip_continuously() {
	pthread_detach(pthread_self());
	while (true) {
		gossip();
		usleep(g_config.gossip_delay * 1000);
	}
}

void gossip() {
	log_t("Hare gossiping con mis seeds a continuacion");

	memory_t* me = (memory_t*) list_find(g_memories, find_myself_as_memory);
	me->timestamp = get_timestamp();

	// Le mando la solicitud de gossiping a cada una de mis memorias
	for (int i = 0; i < list_size(g_config.seed_ips); i++) {
		char* memory_ip = (char*) list_get(g_config.seed_ips, i);
		int* memory_port = (int*) list_get(g_config.seed_ports, i);

		memory_t* memory = malloc(sizeof(memory_t));
		memory->ip = strdup(memory_ip);
		memory->port = *memory_port;

		elements_network_t element_info = elements_gossip_info(g_memories);
		do_simple_request(MEMORY, memory->ip, memory->port, GOSSIP_IN, g_memories, element_info.elements, element_info.elements_size,
				post_gossip, true, NULL, memory);
	}


}

void post_gossip(void* result, response_t* response) {
	memory_t* requested_memory = (memory_t*) response;
	t_list* child_memories = (t_list*) result;
	// Aca tengo que hacer algo con la lista de mi hijo
	if (result == NULL) {
		// Mi hijo no responde.. peor final
		// Me fijo si lo encuentro en la lista padre
		// Y si no lo encuentro, no pasa naranja, ni reviso.
		for (int i = 0; i < list_size(g_memories); i++) {
			memory_t* original = (memory_t*) list_get(g_memories, i);
			if (string_equals_ignore_case(original->ip, requested_memory->ip) && original->port == requested_memory->port) {
				// Esta es la memoria que se cayo
				original->alive = false;
				original->timestamp = get_timestamp();
				break;
			}
		}
	} else {
		for (int i = 0; i < list_size(child_memories); i++) {
			memory_t* memory = (memory_t*) list_get(child_memories, i);
			update_memory(memory);
		}
	}

	log_t("Tabla de memorias tras enviar request de gossiping");
	for (int i = 0; i < list_size(g_memories); i++) {
		memory_t* mem = list_get(g_memories, i);
		char* state = mem->alive ? "alive" : "dead";
		log_t("Id: %i - %s:%i - State: %s - Last update: %lld", mem->id, mem->ip, mem->port, state, mem->timestamp);
	}
	log_t("Fin de tabla");

	free(requested_memory->ip);
	free(requested_memory);
}

void on_gossip(t_list* result, response_t* response) {
	// Aca me llega lo que mi padre me trajo
	for (int i = 0; i < list_size(result); i++) {
		memory_t* memory = (memory_t*) list_get(result, i);
		update_memory(memory);
	}

	t_list* memories_to_send = list_create();
	for (int i = 0; i < list_size(g_memories); i++) {
		memory_t* existing = (memory_t*) list_get(g_memories, i);
		memory_t* new = malloc(sizeof(memory_t));
		new->id = existing->id;
		new->ip = strdup(existing->ip);
		new->port = existing->port;
		new->alive = existing->alive;
		new->timestamp = existing->timestamp;

		list_add(memories_to_send, new);
	}

	log_t("Tabla de memorias tras recibir request de gossiping");
	for (int i = 0; i < list_size(g_memories); i++) {
		memory_t* mem = list_get(g_memories, i);
		char* state = mem->alive ? "alive" : "dead";
		log_t("Id: %i - %s:%i - State: %s - Last update: %lld", mem->id, mem->ip, mem->port, state, mem->timestamp);
	}
	log_t("Fin de tabla");

	set_response(response, memories_to_send);
}

void update_memory(memory_t* memory) {
	memory_t* existing_memory;
	bool found = false;
	sem_wait(&g_mutex_memories);
	for (int i = 0; i < list_size(g_memories); i++) {
		existing_memory = (memory_t*) list_get(g_memories, i);

		// Me fijo que sea la misma memoria y en estado mas actualizado. EN ese caso, la cambio.
		if (existing_memory->id == memory->id) {
			// Existe. Bien, ahora si es una version mas nueva, lo actualizamos
			if (existing_memory->timestamp < memory->timestamp) {
				free(existing_memory->ip);
				existing_memory->ip = strdup(memory->ip);
				existing_memory->port = memory->port;
				existing_memory->alive = memory->alive;
				existing_memory->timestamp = memory->timestamp;
			}
			found = true;
			break;
		}
	}

	// No lo encontre, lo agrego
	if (!found && memory->id != -1) {
		memory_t* new_memory = malloc(sizeof(memory_t));
		new_memory->id = memory->id;
		new_memory->ip = strdup(memory->ip);
		new_memory->port = memory->port;
		new_memory->alive = memory->alive;
		new_memory->timestamp = memory->timestamp;

		list_add(g_memories, new_memory);
	}
	sem_post(&g_mutex_memories);
}

bool find_myself_as_memory(void* elem) {
	memory_t* mem = (memory_t*) elem;
	return mem->id == g_config.memory_number;
}
