#include "metrics.h"

int get_read_latency(consistency_t consistency) {
	return get_operation_latency(consistency, SELECT_EVENT);
}

int get_write_latency(consistency_t consistency) {
	return get_operation_latency(consistency, INSERT_EVENT);
}

int get_reads(consistency_t consistency) {
	return get_operation_count(consistency, SELECT_EVENT);
}

int get_writes(consistency_t consistency) {
	return get_operation_count(consistency, INSERT_EVENT);
}

int get_memory_loads(int memory_id) { // TODO: adecuar para ser mas mejor
	return list_size(g_stats_events);
}

int get_operation_latency(consistency_t consistency, stats_event_t event_t) {
	long current_time_sum = 0;
	int counter = 0;
	for (int i = 0; i < list_size(g_stats_events); i++) {
		stats_t* event = list_get(g_stats_events, i);
		if (event->event_type == event_t && event->consistency == consistency) {
			long differential = timestamp_difference(event->timestamp_start, event->timestamp_end);
			current_time_sum += differential;
			counter++;
		}
	}

	if (counter == 0)
		return 0;
	return current_time_sum / counter;
}

int get_operation_count(consistency_t consistency, stats_event_t event_t) {
	int counter = 0;
	for (int i = 0; i < list_size(g_stats_events); i++) {
		stats_t* event = list_get(g_stats_events, i);
		if (event->event_type == event_t && event->consistency == consistency)
			counter++;
	}

	return counter;
}

void init_metrics() {
	pthread_t metrics_thread;

	if (pthread_create(&metrics_thread, NULL, (void*) process_metrics_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de metricas");
	}
}

void process_metrics() {
	clear_old_stats();
	log_i("                               METRICAS DE KERNEL                               ");
	log_i("--------------------------------------------------------------------------------");
	log_i("   Las metricas estan calculadas en base a eventos de los ultimos 30 segundos   ");
	log_i("  RL: read latency/30s - WL: write latency/30s - R: reads/30s - W: writes/30s.  ");
	log_i("------------------------------ STRONG CONSISTENCY ------------------------------");
	log_i(" RL: %i ms - WL: %i ms - R: %i ops - W: %i ops", get_read_latency(STRONG_CONSISTENCY),
			get_write_latency(STRONG_CONSISTENCY), get_reads(STRONG_CONSISTENCY), get_writes(STRONG_CONSISTENCY));
	log_i("---------------------------- STRONG HASH CONSISTENCY ---------------------------");
	log_i(" RL: %i ms - WL: %i ms - R: %i ops - W: %i ops", get_read_latency(STRONG_HASH_CONSISTENCY),
			get_write_latency(STRONG_HASH_CONSISTENCY), get_reads(STRONG_HASH_CONSISTENCY), get_writes(STRONG_HASH_CONSISTENCY));
	log_i("----------------------------- EVENTUAL CONSISTENCY -----------------------------");
	log_i(" RL: %i ms - WL: %i ms - R: %i ops - W: %i ops", get_read_latency(EVENTUAL_CONSISTENCY),
			get_write_latency(EVENTUAL_CONSISTENCY), get_reads(EVENTUAL_CONSISTENCY), get_writes(EVENTUAL_CONSISTENCY));
	log_i("--------------------------------- MEMORY LOADS ---------------------------------");
	log_i("   Muestra el porcentaje de operaciones en esa memoria sobre el total. La suma  ");
	log_i(" siempre deberia dar el 100%. Solo se muestran memorias con operaciones en los  ");
	log_i("          ultimos 30 segundos (en los que se considera su memory load)          ");
	// TODO: loop y obtener las weas.
	log_i("--------------------------------------------------------------------------------");
}


void process_metrics_continuously() {
	pthread_detach(pthread_self());

	while (true) {
		usleep(g_config.metrics_display * 1000);
		process_metrics();
	}
}
