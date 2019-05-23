#include "stats.h"

#ifndef CORE_STATISTICS_METRICS_H_
#define CORE_STATISTICS_METRICS_H_

long double get_read_latency();
long double get_write_latency();
int get_reads();
int get_writes();
int get_memory_loads();
long double get_operation_latency(stats_event_t event);
long double get_operation_count(stats_event_t event);

#endif /* CORE_STATISTICS_METRICS_H_ */
