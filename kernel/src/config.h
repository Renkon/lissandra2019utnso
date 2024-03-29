#ifndef CONFIG4_H_
#define CONFIG4_H_

#include "kernel.h"
#include "commons/config.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void initialize_kernel_config(t_config* config);
void update_kernel_config(t_config* config);

#endif /* CONFIG_H_ */
