#ifndef COMPACTOR_COMPACTOR_H_
#define COMPACTOR_COMPACTOR_H_

#include <stdio.h>
#include "../lissandra/lissandra.h"
#include <commons/collections/list.h>
#include "../filesystem/filesystem.h"
#include "utils/numbers.h"

void dump();
void dump_table(table_t* table);
int search_free_blocks_for_table(t_list* tkvs);
int tkv_total_length(t_list* tkvs);





#endif /* COMPACTOR_COMPACTOR_H_ */
