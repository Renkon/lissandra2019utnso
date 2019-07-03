#include "gossiping.h"

void init_gossiping() {
	pthread_t gossiping_thread;

	g_memories = list_create();

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

}
