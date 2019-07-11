#ifndef CORE_GOSSIPING_GOSSIPING_H_
#define CORE_GOSSIPING_GOSSIPING_H_

#include "commons/collections/list.h"
#include "commons/string.h"
#include "generic_client.h"
#include "../../config.h"
#include "utils/network_types.h"
#include "utils/serializer.h"
#include "utils/response.h"
#include "utils/dates.h"
#include "utils/numbers.h"
#include "shared_types/shared_types.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "../memories/memories.h"

void init_gossiping();
void gossip_continuously();
void gossip();
void post_gossip(void* result, response_t* response);
void update_memory(memory_t* memory);

#endif /* CORE_GOSSIPING_GOSSIPING_H_ */
