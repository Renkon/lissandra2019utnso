#ifndef CORE_MEMORIES_MEMORIES_H_
#define CORE_MEMORIES_MEMORIES_H_

#include "commons/collections/list.h"
#include "utils/response.h"
#include "shared_types/shared_types.h"
#include "generic_logger.h"
#include "../scheduler/pcb.h"
#include "utils/numbers.h"
#include <stdlib.h>
#include <stdint.h>

t_list* g_memories_added_sc;
t_list* g_memories_added_shc;
t_list* g_memories_added_ec;

t_list* g_memories;

int ec_next;

void init_memory_list();

memory_t* get_any_memory();
memory_t* get_any_sc_memory();
memory_t* get_any_shc_memory(uint16_t key);
memory_t* get_any_ec_memory();

void add_sc_memory(int id);
void add_shc_memory(int id);
void add_ec_memory(int id);

void remove_memory(int id);

memory_t* get_memory_by_id(t_list* mem_list, int id);
void remove_memory_from_consistency(t_list* mems, int id);
bool is_memory_alive(void* memory);
memory_t* get_random_memory(bool alive);

void journaling(bool only_shc, void (*callback)(void*, response_t*), pcb_t* pcb);

t_list* get_list_with_a_consistency();

#endif /* CORE_MEMORIES_MEMORIES_H_ */
