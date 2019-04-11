#include "filesystem.h"

// Las keys de configuracion
char* g_config_keys[] = { "PUERTO_ESCUCHA", "PUNTO_MONTAJE", "RETARDO", "TAMAÑO_VALUE", "TIEMPO_DUMP" };
int g_config_keys_size = 5;

int main(void) {
	if (!init_logger("filesystem.log", "Filesystem", true, LOG_LEVEL_TRACE))
		return 1;

	init_config(FSCFG, initialize_fs_config, update_fs_config, g_config_keys, g_config_keys_size);
	init_server(g_config.port);
	init_console("Papito codeo en Assembler - Filesystem v1.0", "filesystem>", FILESYSTEM, get_callbacks());
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return build_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, NULL, NULL, NULL);
}
