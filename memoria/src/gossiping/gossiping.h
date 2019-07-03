#include "commons/collections/list.h"
#include "generic_client.h"
#include "../config.h"
#include "utils/network_types.h"
#include "utils/serializer.h"
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
void post_gossip(void* result, response_t* response);
void on_gossip(t_list* received, response_t* response);
#endif /* GOSSIPING_GOSSIPING_H_ */
