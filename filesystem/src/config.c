#include "config.h"
#define EVENT_SIZE    ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

void initialize_fs_config(t_config* config) {
	char* mount_point = config_get_string_value(config, "PUNTO_MONTAJE");
	g_config.mount_point = malloc(strlen(mount_point));
	strcpy(g_config.mount_point, mount_point);

	g_config.port = config_get_int_value(config, "PUERTO_ESCUCHA");
	g_config.delay = config_get_int_value(config, "RETARDO");
	g_config.max_value_size = config_get_int_value(config, "TAMAÑO_VALUE");
	g_config.dump_time = config_get_int_value(config, "TIEMPO_DUMP");
}

void update_fs_config(t_config* config) {
	char* new_mount_point = config_get_string_value(config, "PUNTO_MONTAJE");
	int new_delay = config_get_int_value(config, "RETARDO");
	int new_max_value_size = config_get_int_value(config, "TAMAÑO_VALUE");
	int new_dump_time = config_get_int_value(config, "TIEMPO_DUMP");

	if (strcmp(new_mount_point, g_config.mount_point) != 0) {
		g_config.mount_point = realloc(g_config.mount_point, strlen(new_mount_point));
		log_info(g_logger, "Nuevo valor de PUNTO_MONTAJE detectado -> %s", strcpy(g_config.mount_point, new_mount_point));
	}
	if (new_delay != g_config.delay)
		log_info(g_logger, "Nuevo valor de RETARDO detectado -> %i", g_config.delay = new_delay);
	if (new_max_value_size != g_config.max_value_size)
		log_info(g_logger, "Nuevo valor de TAMAÑO_VALUE detectado -> %i", g_config.max_value_size = new_max_value_size);
	if (new_dump_time != g_config.dump_time)
		log_info(g_logger, "Nuevo valor de TIEMPO_DUMP detectado -> %i", g_config.dump_time = new_dump_time);
}
