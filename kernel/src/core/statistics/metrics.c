#include "metrics.h"

long double get_read_latency() {
	return get_operation_latency(SELECT_EVENT);
}

long double get_write_latency() {
	return get_operation_latency(INSERT_EVENT);
}

int get_reads() {
	return get_operation_count(SELECT_EVENT);
}

int get_writes() {
	return get_operation_count(INSERT_EVENT);
}

int get_memory_loads() { // TODO: adecuar para ser mas mejor
	return list_size(g_stats_events);
}

long double get_operation_latency(stats_event_t event_t) {
	long long current_time_sum = 0;
	int counter = 0;
	for (int i = 0; i < list_size(g_stats_events); i++) {
		stats_t* event = list_get(g_stats_events, i);
		if (event->event_type == event_t) {
			long long differential = timestamp_difference(event->timestamp_start, event->timestamp_end);
			current_time_sum += differential;
			counter++;
		}
	}

	if (counter == 0)
		return 0;
	return current_time_sum / counter;
}

long double get_operation_count(stats_event_t event_t) {
	int counter = 0;
	for (int i = 0; i < list_size(g_stats_events); i++) {
		stats_t* event = list_get(g_stats_events, i);
		if (event->event_type == event_t)
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
	// TODO: hay que hacerlas por cada CRITERIO!!!
	clear_old_stats();
	printf("                               METRICAS DE KERNEL                               \n");
	printf("--------------------------------------------------------------------------------\n");
	printf(" - 1> Read latency / 30s: %Lf\n", get_read_latency());
	printf(" - 2> Write latency / 30s: %Lf\n", get_write_latency());
	printf(" - 3> Reads / 30s: %i\n", get_reads());
	printf(" - 4> Writes / 30s: %i\n", get_writes());
	// TODO: get_memory_loads();
	printf("--------------------------------------------------------------------------------\n");
}


void process_metrics_continuously() {
	while (true) {
		usleep(30000 * 1000);
		process_metrics();
	}
}
