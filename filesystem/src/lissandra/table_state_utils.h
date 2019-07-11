#ifndef LISSANDRA_TABLE_STATE_UTILS_H_
#define LISSANDRA_TABLE_STATE_UTILS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <semaphore.h>

typedef struct {
	char*	name;
	sem_t* is_blocked_mutex;
	sem_t* live_status_mutex;
	pthread_t compaction_thread;

}table_state_t;



#endif /* LISSANDRA_TABLE_STATE_UTILS_H_ */
