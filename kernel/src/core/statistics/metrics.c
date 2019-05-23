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
