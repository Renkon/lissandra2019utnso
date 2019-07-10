#include <stdlib.h>
#include "commons/collections/list.h"
#include "generic_logger.h"
#include "utils/operation_types.h"
#include "utils/dates.h"
#include <semaphore.h>

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

typedef struct {
	int memory_id;
	int requests;
} historic_t;

sem_t g_stats_semaphore;

t_list* g_stats_events;
t_list* g_historic_infos;

void clear_old_stats();

void add_to_historics(int memory_id);
int get_historic_requests(int memory_id);
int get_total_requests(int memory_id);
int get_total_requests_overall(t_list* memories);

#endif /* CORE_STATISTICS_STATS_H_ */
