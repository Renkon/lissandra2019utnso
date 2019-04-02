#include <pthread.h>
#include "config.h"

// Las keys de configuracion
char* g_config_keys[] = { "PUERTO_ESCUCHA", "PUNTO_MONTAJE", "RETARDO", "TAMAÑO_VALUE", "TIEMPO_DUMP" };
int g_config_keys_size = 5;

int main(void) {
	g_logger = log_create("filesystem.log", "Filesystem", true, LOG_LEVEL_TRACE);

	if (g_logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		perror("No se pudo configurar logger. Abortando ejecucion");
		return 1;
	}

	init_config(FSCFG, initialize_fs_config, update_fs_config, g_config_keys, g_config_keys_size, g_logger);

	log_destroy(g_logger);
	return 0;
}
