#include "config.h"

void initialize_fs_config(t_config* config) {
	g_config.mount_point = init_str_config_value("PUNTO_MONTAJE", config);
	g_config.port = init_int_config_value("PUERTO_ESCUCHA", config);
	g_config.delay = init_int_config_value("RETARDO", config);
	g_config.max_value_size = init_int_config_value("TAMAÑO_VALUE", config);
	g_config.dump_time = init_int_config_value("TIEMPO_DUMP", config);
}

void update_fs_config(t_config* config) {
	g_config.mount_point = update_str_config_value(g_config.mount_point, "PUNTO_MONTAJE", config);
	g_config.delay = update_int_config_value(g_config.delay, "RETARDO", config);
	g_config.max_value_size = update_int_config_value(g_config.max_value_size, "TAMAÑO_VALUE", config);
	g_config.dump_time = update_int_config_value(g_config.dump_time, "TIEMPO_DUMP", config);
}
