#ifndef CORE_UTILS_PID_UTILS_H_
#define CORE_UTILS_PID_UTILS_H_

#include <stdlib.h>
#include "commons/collections/list.h"

t_list* g_process_ids;

void setup_pid_generator();
int generate_pid();
void remove_pid(int id);

#endif /* CORE_UTILS_PID_UTILS_H_ */
