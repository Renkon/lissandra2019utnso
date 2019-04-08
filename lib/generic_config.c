#include "generic_config.h"

#define EVENT_SIZE    ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

bool init_config(char* config_file, void (*callback_created)(t_config*), void (*callback_updated)(t_config*), char** config_keys, int config_size, t_log* logger) {
	t_config* config = config_create(config_file);
	config_args_t* thread_args;
	pthread_t config_thread;

	if (!validate_config(config, config_keys, config_size, logger))
		return false;

	callback_created(config);

	config_destroy(config);

	thread_args = build_config_args(config_keys, config_size, config_file, logger, callback_updated);

	if (pthread_create(&config_thread, NULL, (void*) pre_check_config, (void*) thread_args)) {
			log_error(logger, "No se pudo inicializar el hilo de configuracion");
			return false;
	}

	log_info(logger, "Se cargo satisfactoriamente la configuracion");

	return true;
}

void update_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger) {
	t_config* config = config_create(config_file);

	if (!validate_config(config, config_keys, config_size, logger)) {
		log_warning(logger, "El archivo de configuracion quedo invalidado. Ignorando nuevos valores");
		return;
	}

	callback(config);

	config_destroy(config);
}

void pre_check_config(config_args_t* args) {
	pthread_detach(pthread_self());

	check_config(args->config_file, args->callback_updated, args->config_keys, args->config_size, args->logger);

	// Esto no se deberia ejecutar, pero eventualmente, si stoppeamos el bucle infinito, deberia liberar la memoria
	free(args);
}

void check_config(char* config_file, void (*callback)(t_config*), char** config_keys, int config_size, t_log* logger) {
	int inotify_fd = inotify_init();
	int file_fd;
	char buffer[EVENT_BUF_LEN];

	if (inotify_fd < 0) {
		log_warning(logger, "No se pudo inicializar inotify. No se estara actualizando el archivo de configuracion");
		return;
	}

	file_fd = inotify_add_watch(inotify_fd, ".", IN_CLOSE_WRITE);

	while (1) {
		int length = read(inotify_fd, buffer, EVENT_BUF_LEN);
		int offset = 0;

		/* Revisamos que haya traido algo.. si no ignoramos */
		if (length > 0) {
			while (offset < length) {
				struct inotify_event *event = (struct inotify_event *) &buffer[offset];
				// Si el archivo modificado es el de configuracion (por el nombre)
				if (event->len && strcmp(event->name, config_file) == 0) {
					update_config(config_file, callback, config_keys, config_size, logger);
				}

				offset += sizeof (struct inotify_event) + event->len;
			}
		}
	}

	log_warning(logger, "Se cerro erroneamente el filedescriptor de inotify. No se estara actualizando el archivo de configuracion");
	inotify_rm_watch(inotify_fd, file_fd);
	close(inotify_fd);
}

bool validate_config(t_config* config, char** config_keys, int config_size, t_log* logger) {
	if (config == NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion");
		return false;
	}

	if (!validate_config_properties(config, config_keys, config_size, logger)) {
		log_error(logger, "El archivo de configuracion no tiene todas las propiedades");
		config_destroy(config);
		return false;
	}

	return true;
}

bool validate_config_properties(t_config* config, char** config_keys, int config_size, t_log* logger) {
	// Esa division de sizeof devuelve magicamente el tamaño del array de strings
	for (int i = 0; i < config_size; i++) {
		if (!config_has_property(config, config_keys[i])) {
			log_error(logger, "No esta definida la propiedad %s", config_keys[i]);
			return false;
		}
	}

	return true;
}

config_args_t* build_config_args(char** config_keys, int config_size, char* file, t_log* logger, void (*updated)(t_config*)) {
	config_args_t* config_args = malloc(sizeof(config_args_t));

	config_args->config_keys = config_keys;
	config_args->config_file = file;
	config_args->config_size = config_size;
	config_args->logger = logger;
	config_args->callback_updated = updated;

	return config_args;
}

int init_int_config_value(char* key, t_config* config, t_log* logger) {
	int value = config_get_int_value(config, key);

	log_info(logger, "Se obtuvo el valor de propiedad %s. Valor -> %i", key, value);

	return value;
}

int update_int_config_value(int current_value, char* key, t_config* config, t_log* logger) {
	int new_value = config_get_int_value(config, key);

	if (new_value != current_value) {
		log_info(logger, "Nuevo valor de %s detectado -> %i", key, new_value);
	}

	return new_value;
}

char* init_str_config_value(char* key, t_config* config, t_log* logger) {
	char* value = config_get_string_value(config, key);
	char* destination = malloc(strlen(value) + 1);

	log_info(logger, "Se obtuvo el valor de propiedad %s. Valor -> %s", key, strcpy(destination, value));

	return destination;
}

char* update_str_config_value(char* old_value, char* key, t_config* config, t_log* logger) {
	char* new_value = config_get_string_value(config, key);
	char* ret_value = old_value;

	if (strcmp(old_value, new_value) != 0) {
		free(old_value);
		ret_value = malloc(strlen(new_value) + 1);
		log_info(logger, "Nuevo valor de %s detectado -> %s", key, strcpy(ret_value, new_value));
	}

	return ret_value;
}

t_list* init_str_array_config_value(char* key, t_config* config, t_log* logger) {
	t_list* values = list_create();
	char** value = config_get_array_value(config, key);
	int currentValue = 0;

	log_info(logger, "Se obtuvo el valor de propiedad %s. Valor tipo lista", key);

	while (*value != NULL) {
		list_add(values, *value);
		log_info(logger, "Valor %s[%i] -> %s", key, currentValue++, *value);
		value++;
	}

	return values;
}

t_list* init_int_array_config_value(char* key, t_config* config, t_log* logger) {
	t_list* str_values = init_str_array_config_value(key, config, logger);
	t_list* values = list_map(str_values, (void*) atol);
	list_destroy(str_values);
	return values;
}

