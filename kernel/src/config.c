#include "config.h"

void initialize_kernel_config(t_config* config) {
	char* memory_ip = config_get_string_value(config, "IP_MEMORIA");
	g_config.memory_ip = malloc(strlen(memory_ip));
	strcpy(g_config.memory_ip, memory_ip);

	g_config.memory_port = config_get_int_value(config, "PUERTO_MEMORIA");
	g_config.quantum = config_get_int_value(config, "QUANTUM");
	g_config.multiprocessing = config_get_int_value(config, "MULTIPROCESAMIENTO");
	g_config.metadata_refresh = config_get_int_value(config, "METADATA_REFRESH");
	g_config.execution_delay = config_get_int_value(config, "SLEEP_EJECUCION");
}

void update_kernel_config(t_config* config) {
	int new_quantum = config_get_int_value(config, "QUANTUM");
	int new_multiprocessing = config_get_int_value(config, "MULTIPROCESAMIENTO");
	int new_metadata_refresh = config_get_int_value(config, "METADATA_REFRESH");
	int new_execution_delay = config_get_int_value(config, "SLEEP_EJECUCION");

	if (new_quantum != g_config.quantum)
		log_info(g_logger, "Nuevo valor de QUANTUM detectado -> %i", g_config.quantum = new_quantum);
	if (new_multiprocessing != g_config.multiprocessing)
		log_info(g_logger, "Nuevo valor de MULTIPROCESAMIENTO detectado -> %i", g_config.multiprocessing = new_multiprocessing);
	if (new_metadata_refresh != g_config.metadata_refresh)
		log_info(g_logger, "Nuevo valor de METADATA_REFRESH detectado -> %i", g_config.metadata_refresh =  new_metadata_refresh);
	if (new_execution_delay != g_config.execution_delay)
		log_info(g_logger, "Nuevo valor de SLEEP_EJECUTION detectado -> %i", g_config.execution_delay =  new_execution_delay);
}
