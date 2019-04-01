#include <pthread.h>
#include "config.h"

// Las keys de configuracion
char* g_config_keys[] = { "PUERTO_ESCUCHA", "PUNTO_MONTAJE", "RETARDO", "TAMAÑO_VALUE", "TIEMPO_DUMP" };

int main(void) {
	g_logger = log_create("filesystem.log", "Filesystem", true, LOG_LEVEL_TRACE);
	config_args_t* config_thread_args = build_config_args();
	pthread_t config_thread;

	if (g_logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		return 1;
	}

	// Creo un hilo que se encarga de lo relacionado a configuracion
	if (pthread_create(&config_thread, NULL, (void*) pre_init_config, (void*) config_thread_args)){
		log_error(g_logger, "No se pudo inicializar el hilo de configuracion");
		return 1;
	}

	pthread_join(config_thread, NULL);
	log_destroy(g_logger);
	return 0;
}

config_args_t* build_config_args() {
	int config_size = 5;

	config_args_t* config_args = malloc(sizeof(config_args_t));

	config_args->config_keys = g_config_keys;
	config_args->config_file = FSCFG;
	config_args->config_size = config_size;
	config_args->logger = g_logger;
	config_args->callback_created = initialize_fs_config;
	config_args->callback_updated = update_fs_config;

	return config_args;
}
