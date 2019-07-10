#include <stdlib.h>
#include "commons/collections/list.h"
#include "generic_logger.h"
#include "utils/operation_types.h"
#include "utils/dates.h"

#ifndef CORE_STATISTICS_STATS_H_
#define CORE_STATISTICS_STATS_H_

typedef enum {
	SELECT_EVENT,
	INSERT_EVENT
} stats_event_t;

typedef struct {
	long long timestamp_start;
	long long timestamp_end;
	int memory;
	stats_event_t event_type;
	consistency_t consistency;
} stats_t;

t_list* g_stats_events;

void clear_old_stats();

#endif /* CORE_STATISTICS_STATS_H_ */
