#include "stats.h"
#include "shared_types/shared_types.h"

void clear_old_stats() {
	// Aca limpiamos las estadisticas antiguas
	int timelimit = 30000; // 30 segundos
	long long current_timestamp = get_timestamp();

	sem_wait(&g_stats_semaphore);
	for (int i = list_size(g_stats_events) - 1; i >= 0; i--) {
		stats_t* event = list_get(g_stats_events, i);

		if (timestamp_difference(current_timestamp, event->timestamp_end) > timelimit) {
			// Borramos el elemento
			add_to_historics(event->memory);
			log_t("Se borro un evento de las estadisticas.");
			list_remove(g_stats_events, i);
			free(event);
		}
	}
	sem_post(&g_stats_semaphore);
}

void add_to_historics(int memory_id) {
	historic_t* historic;
	for (int i = 0; i < list_size(g_historic_infos); i++) {
		historic = (historic_t*) list_get(g_historic_infos, i);

		if (historic->memory_id == memory_id) {
			historic->requests++;
			return;
		}
	}

	historic = malloc(sizeof(historic_t));
	historic->memory_id = memory_id;
	historic->requests = 1;
	list_add(g_historic_infos, historic);
}

int get_historic_requests(int memory_id) {
	for (int i = 0; i < list_size(g_historic_infos); i++) {
		historic_t* historic = (historic_t*) list_get(g_historic_infos, i);

		if (historic->memory_id == memory_id)
			return historic->requests;
	}

	return 0;
}

int get_total_requests(int memory_id) {
	int requests = get_historic_requests(memory_id);
	sem_wait(&g_stats_semaphore);

	for (int i = 0; i < list_size(g_stats_events); i++) {
		stats_t* event = (stats_t*) list_get(g_stats_events, i);
		if (event->memory == memory_id)
			requests++;
	}

	sem_post(&g_stats_semaphore);
	return requests;
}

int get_total_requests_overall(t_list* memories) {
	int total = 0;

	for (int i = 0; i < list_size(memories); i++) {
		memory_t* memory = (memory_t*) list_get(memories, i);
		total += get_total_requests(memory->id);
	}

	return total;
}
