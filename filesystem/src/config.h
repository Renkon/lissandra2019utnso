#ifndef CONFIG2_H_
#define CONFIG2_H_

#include "commons/config.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "lissandra/lissandra.h"

void initialize_fs_config(t_config* config);
void update_fs_config(t_config* config);

#endif /* CONFIG2_H_ */
