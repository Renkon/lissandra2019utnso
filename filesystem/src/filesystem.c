#include <pthread.h>
#include "filesystem.h"
#include "config.h"

int main(void) {
	g_logger = log_create("filesystem.log", "Filesystem", true, LOG_LEVEL_TRACE);
	pthread_t config_thread;

	if (g_logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		return 1;
	}

	if (pthread_create(&config_thread, NULL, (void*) init_config, NULL)){
		log_error(g_logger, "No se pudo inicializar el hilo de configuracion");
		return 1;
	}

	pthread_join(config_thread, NULL);
	log_destroy(g_logger);
	return 0;
}
