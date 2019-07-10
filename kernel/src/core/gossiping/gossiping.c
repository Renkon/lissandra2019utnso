#include "gossiping.h"

void init_gossiping() {
	pthread_t gossiping_thread;
	srandom(time(NULL));

	g_memories = list_create();

	memory_t* main_memory = malloc(sizeof(memory_t));
	main_memory->alive = true;
	main_memory->id = -1; // usado solo hasta que termina el primer gossiping
	main_memory->ip = strdup(g_config.memory_ip);
	main_memory->port = g_config.memory_port;
	main_memory->timestamp = get_timestamp();

	list_add(g_memories, main_memory);

	if (pthread_create(&gossiping_thread, NULL, (void*) gossip_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de gossiping");
	}
}

void gossip_continuously() {
	pthread_detach(pthread_self());
	while (true) {
		usleep(g_config.gossip_refresh * 1000);
		gossip();
	}
}

void gossip() {
	log_t("Iniciando request de gossiping");

	memory_t* memory = get_random_memory(true);

	if (memory == NULL) { // no hay ninguna viva.
		log_e("Algo anda mal! No hay memorias accesibles!");
		log_e("Por esto, eligire una memoria muerta al azar, para ver si esta viva");
		memory = get_random_memory(false);
	}

	elements_network_t element_info = elements_gossip_info(g_memories);

	do_simple_request(KERNEL, memory->ip, memory->port, GOSSIP_IN, g_memories, element_info.elements, element_info.elements_size,
			post_gossip, true, NULL, memory);

	log_t("Se disparo request de gossiping");
}

void post_gossip(void* result, response_t* response) {
	memory_t* selected_memory = (memory_t*) response;
	t_list* received_memories = (t_list*) result;

	if (result == NULL) { // Fallo la request?
		selected_memory->alive = false;
		selected_memory->timestamp = get_timestamp();
		remove_memory(selected_memory->id); // la borro de las tablas de consistencias
	} else {
		// Se hizo la ricuest! Caso inicial
		// Estoy en la memoria -1 (la principal sin ID)
		if (selected_memory->id == -1) {
			list_remove(g_memories, 0); // chau!
			free(selected_memory->ip);
			free(selected_memory);
		} else {
			selected_memory->alive = true;
			selected_memory->timestamp = get_timestamp();
		}

		// Bueno, entonces aca si, lo que recibimos lo anexamos a la tabla de memorias
		for (int i = 0; i < list_size(received_memories); i++) {
			memory_t* mem = (memory_t*) list_get(received_memories, i);
			update_memory(mem);
		}
	}

	log_t("Tabla de memorias tras haber hecho gossiping");
	for (int i = 0; i < list_size(g_memories); i++) {
		memory_t* mem = list_get(g_memories, i);
		char* state = mem->alive ? "alive" : "dead";
		log_t("Id: %i - %s:%i - State: %s - Last update: %lld", mem->id, mem->ip, mem->port, state, mem->timestamp);
	}
	log_t("Fin de tabla");
}

void update_memory(memory_t* memory) {
	memory_t* existing_memory;
	bool found = false;

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

				if (!existing_memory->alive) // la borro de todos los criterios
					remove_memory(existing_memory->id);
			}
			found = true;
			break;
		}
	}

	// No lo encontre, lo agrego
	if (!found) {
		memory_t* new_memory = malloc(sizeof(memory_t));
		new_memory->id = memory->id;
		new_memory->ip = strdup(memory->ip);
		new_memory->port = memory->port;
		new_memory->alive = memory->alive;
		new_memory->timestamp = memory->timestamp;

		list_add(g_memories, new_memory);
	}
}

