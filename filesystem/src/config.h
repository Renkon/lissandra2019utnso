#ifndef CONFIG2_H_
#define CONFIG2_H_

#include "lissandra/filesystem.h"
#include "commons/config.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void initialize_fs_config(t_config* config);
void update_fs_config(t_config* config);

#endif /* CONFIG2_H_ */
