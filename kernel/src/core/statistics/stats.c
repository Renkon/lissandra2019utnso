#include "stats.h"

void clear_old_stats() {
	// Aca limpiamos las estadisticas antiguas
	int timelimit = 30000; // 30 segundos
	long long current_timestamp = get_timestamp();

	for (int i = list_size(g_stats_events) - 1; i >= 0; i--) {
		stats_t* event = list_get(g_stats_events, i);

		if (timestamp_difference(current_timestamp, event->timestamp_end) > timelimit) {
			// Borramos el elemento
			log_t("Se borro un evento de las estadisticas.");
			list_remove(g_stats_events, i);
			free(event);
		}
	}
}
