#include "memoria.h"


// Las keys de configuracion de memoria
char* g_config_keys[] = { "PUERTO", "IP_FS", "PUERTO_FS", "IP_SEEDS", "PUERTO_SEEDS","RETARDO_MEM",
"RETARDO_FS","TAM_MEM","RETARDO_JOURNAL","RETARDO_GOSSIPING","MEMORY_NUMBER"};
int g_config_keys_size = 11;

int main(void) {
	g_logger = log_create("memoria.log", "Memoria", true, LOG_LEVEL_TRACE);

	if (g_logger == NULL) {
		// No pudimos crear el logger, asi que no podemos continuar
		perror("No se pudo configurar logger. Abortando ejecucion");
		return 1;
	}

	init_config(MEMCFG, initialize_memory_config, update_memory_config, g_config_keys, g_config_keys_size, g_logger);
	init_console("Papito codeo en Assembler - Memoria v1.0", "memory>", MEMORY, g_logger);
	log_destroy(g_logger);
	return 0;
}
