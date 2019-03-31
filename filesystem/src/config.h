#ifndef CONFIG2_H_
#define CONFIG2_H_
#define FSCFG "filesystem.cfg"

#include "filesystem.h"
#include "commons/config.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void init_config(void*);
void update_config();
void check_config();
bool validate_config(t_config* config);
bool validate_config_properties(t_config* config);

#endif /* CONFIG2_H_ */
