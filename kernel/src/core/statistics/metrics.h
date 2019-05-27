#include "stats.h"
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#ifndef CORE_STATISTICS_METRICS_H_
#define CORE_STATISTICS_METRICS_H_

long double get_read_latency();
long double get_write_latency();
int get_reads();
int get_writes();
int get_memory_loads();
long double get_operation_latency(stats_event_t event);
long double get_operation_count(stats_event_t event);
void init_metrics();
void process_metrics();
void process_metrics_continuously();

#endif /* CORE_STATISTICS_METRICS_H_ */
