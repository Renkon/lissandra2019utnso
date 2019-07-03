#include "commons/collections/list.h"
#include "generic_client.h"
#include "../config.h"
#include "utils/network_types.h"
#include "shared_types/shared_types.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef GOSSIPING_GOSSIPING_H_
#define GOSSIPING_GOSSIPING_H_

t_list* g_memories;

void init_gossiping();
void gossip_continuously();
void gossip();

#endif /* GOSSIPING_GOSSIPING_H_ */
