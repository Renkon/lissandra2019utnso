#include "config.h"

void initialize_kernel_config(t_config* config) {
	g_config.memory_ip = init_str_config_value("IP_MEMORIA", config);
	g_config.memory_port = init_int_config_value("PUERTO_MEMORIA", config);
	g_config.quantum = init_int_config_value("QUANTUM", config);
	g_config.multiprocessing = init_int_config_value("MULTIPROCESAMIENTO", config);
	g_config.metadata_refresh = init_int_config_value("METADATA_REFRESH", config);
	g_config.execution_delay = init_int_config_value("SLEEP_EJECUCION", config);
	g_config.lts_delay = init_int_config_value("LTS_DELAY", config);
}

void update_kernel_config(t_config* config) {
	g_config.quantum = update_int_config_value(g_config.quantum, "QUANTUM", config);
	g_config.metadata_refresh = update_int_config_value(g_config.metadata_refresh, "METADATA_REFRESH", config);
	g_config.execution_delay = update_int_config_value(g_config.execution_delay, "SLEEP_EJECUCION", config);
	g_config.lts_delay = update_int_config_value(g_config.lts_delay, "LTS_DELAY", config);
}
