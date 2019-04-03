#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "config.h"

// Las keys de configuracion... pero ahora del kernel
char* g_config_keys[] = { "IP_MEMORIA", "PUERTO_MEMORIA", "QUANTUM", "MULTIPROCESAMIENTO", "METADATA_REFRESH","SLEEP_EJECUCION" };
int g_config_keys_size = 6;

int main(void) {
	g_logger= log_create("kernel.log", "Kernel", true, LOG_LEVEL_TRACE);

	if (g_logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		perror("No se pudo configurar logger. Abortando ejecucion");
		return 1;
	}

	init_config(KNCFG, initialize_kernel_config, update_kernel_config, g_config_keys, g_config_keys_size, g_logger);

	log_destroy(g_logger);
	return 0;
}
