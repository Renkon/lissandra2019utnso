#include "config.h"
#define EVENT_SIZE    ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

void init_config(void* unused) {
	t_config* config = config_create(FSCFG);

	if (!validate_config(config))
		return;

	g_config.port = config_get_int_value(config, "PUERTO_ESCUCHA");
	strcpy(g_config.mount_point, config_get_string_value(config, "PUNTO_MONTAJE"));
	g_config.delay = config_get_int_value(config, "RETARDO");
	g_config.max_value_size = config_get_int_value(config, "TAMAÑO_VALUE");
	g_config.dump_time = config_get_int_value(config, "TIEMPO_DUMP");

	config_destroy(config);

	log_info(g_logger, "Se cargo satisfactoriamente la configuracion");

	check_config();
}

void update_config() {
	t_config* config = config_create(FSCFG);

	if (!validate_config(config)) {
		log_warning(g_logger, "El archivo de configuracion quedo invalidado. Ignorando nuevos valores");
		return;
	}

	char* new_mount_point = config_get_string_value(config, "PUNTO_MONTAJE");
	int new_delay = config_get_int_value(config, "RETARDO");
	int new_max_value_size = config_get_int_value(config, "TAMAÑO_VALUE");
	int new_dump_time = config_get_int_value(config, "TIEMPO_DUMP");

	if (strcmp(new_mount_point, g_config.mount_point) != 0)
		log_info(g_logger, "Nuevo valor de PUNTO_MONTAJE detectado -> %s", strcpy(g_config.mount_point, new_mount_point));
	if (new_delay != g_config.delay)
		log_info(g_logger, "Nuevo valor de RETARDO detectado -> %i", g_config.delay = new_delay);
	if (new_max_value_size != g_config.max_value_size)
		log_info(g_logger, "Nuevo valor de TAMAÑO_VALUE detectado -> %i", g_config.max_value_size = new_max_value_size);
	if (new_dump_time != g_config.dump_time)
		log_info(g_logger, "Nuevo valor de TIEMPO_DUMP detectado -> %i", g_config.dump_time = new_dump_time);

	config_destroy(config);
}

void check_config() {
	int inotify_fd = inotify_init();
	int file_fd;
	char buffer[EVENT_BUF_LEN];

	if (inotify_fd < 0) {
		log_warning(g_logger, "No se pudo inicializar inotify. No se estara actualizando el archivo de configuracion");
		return;
	}

	file_fd = inotify_add_watch(inotify_fd, ".", IN_CLOSE_WRITE);

	for (;;) {
		int length = read(inotify_fd, buffer, EVENT_BUF_LEN);
		int offset = 0;

		/* Revisamos que haya traido algo.. si no ignoramos */
		if (length > 0) {
			while (offset < length) {
				struct inotify_event *event = (struct inotify_event *) &buffer[offset];
				// Si el archivo modificado es el de configuracion (por el nombre)
				if (event->len && strcmp(event->name, FSCFG) == 0) {
					update_config();
				}

				offset += sizeof (struct inotify_event) + event->len;
			}
		}
	}

	log_warning(g_logger, "Se cerro erroneamente el filedescriptor de inotify. No se estara actualizando el archivo de configuracion");
	inotify_rm_watch(inotify_fd, file_fd);
	close(inotify_fd);
}

bool validate_config(t_config* config) {
	if (config == NULL) {
		log_error(g_logger, "No se pudo abrir el archivo de configuracion");
		return false;
	}

	if (!validate_config_properties(config)) {
		log_error(g_logger, "El archivo de configuracion no tiene todas las propiedades");
		config_destroy(config);
		return false;
	}

	return true;
}

bool validate_config_properties(t_config* config) {
	char* config_keys[] = { "PUERTO_ESCUCHA", "PUNTO_MONTAJE", "RETARDO", "TAMAÑO_VALUE", "TIEMPO_DUMP" };

	// Esa division de sizeof devuelve magicamente el tamaño del array de strings
	for (int i = 0; i < (sizeof(config_keys) / sizeof(config_keys[0])); i++) {
		if (!config_has_property(config, config_keys[i])) {
			log_error(g_logger, "No esta definida la propiedad %s", config_keys[i]);
			return false;
		}
	}

	return true;
}
