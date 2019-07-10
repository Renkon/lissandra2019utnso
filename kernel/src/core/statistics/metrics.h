#include "stats.h"
#include "../../config.h"
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "utils/operation_types.h"

#ifndef CORE_STATISTICS_METRICS_H_
#define CORE_STATISTICS_METRICS_H_

int get_read_latency(consistency_t consistency);
int get_write_latency(consistency_t consistency);
int get_reads(consistency_t consistency);
int get_writes(consistency_t consistency);
int get_memory_loads(int memory_id);
int get_operation_latency(consistency_t consistency, stats_event_t event);
int get_operation_count(consistency_t consistency, stats_event_t event);
void init_metrics();
void process_metrics();
void process_metrics_continuously();

#endif /* CORE_STATISTICS_METRICS_H_ */
