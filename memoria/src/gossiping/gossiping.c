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

	if (pthread_create(&gossiping_thread, NULL, (void*) gossip_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de describe");
	}
}

void gossip_continuously() {
	while (true) {
		usleep(g_config.gossip_delay * 1000);
		gossip();
	}
}

void gossip() {
	// Le mando la solicitud de gossiping a cada una de mis memorias
	for (int i = 0; i < list_size(g_config.seed_ips); i++) {
		char* memory_ip = (char*) list_get(g_config.seed_ips, i);
		int* memory_port = (int*) list_get(g_config.seed_ports, i);
		elements_network_t element_info = elements_gossip_info(g_memories);
		do_simple_request(MEMORY, memory_ip, *memory_port, GOSSIP_IN, g_memories, element_info.elements, element_info.elements_size,
				post_gossip, true, NULL, NULL);
	}
}

void post_gossip(void* result, response_t* response) {
	t_list* child_memories = (t_list*) result;
	// Aca tengo que hacer algo con la lista q me llega
	// TODO: falta serializacion/deserializacion
}

void on_gossip(t_list* result, response_t* response) {
	// Aca me llega la lista que me trae mi padre para gossipear como un capo
	// TODO: falta serializacion/deserializacion
}
