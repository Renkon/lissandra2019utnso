/*
 * stats.h
 *
 *  Created on: 5 may. 2019
 *      Author: utnso
 */

#ifndef CORE_STATISTICS_STATS_H_
#define CORE_STATISTICS_STATS_H_

typedef struct {
	long timestamp_start;
	long timestamp_end;
	int memory;
	stats_event_t event_type;
} stats_t;

typedef enum {
	SELECT,
	INSERT
} stats_event_t;

t_list* g_stats_events;

#endif /* CORE_STATISTICS_STATS_H_ */
