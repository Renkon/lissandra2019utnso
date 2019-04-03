#ifndef CONFIG3_H_
#define CONFIG3_H_

#include "memoria.h"
#include "commons/config.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void initialize_memory_config(t_config* config);
void update_memory_config(t_config* config);

#endif /* CONFIG3_H_ */
