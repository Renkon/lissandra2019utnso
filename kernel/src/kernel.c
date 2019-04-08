#include "kernel.h"

// Las keys de configuracion... pero ahora del kernel
char* g_config_keys[] = { "IP_MEMORIA", "PUERTO_MEMORIA", "QUANTUM", "MULTIPROCESAMIENTO", "METADATA_REFRESH","SLEEP_EJECUCION" };
int g_config_keys_size = 6;

int main(void) {
	if (!init_logger("kernel.log", "Kernel", true, LOG_LEVEL_TRACE))
		return 1;
	init_config(KNCFG, initialize_kernel_config, update_kernel_config, g_config_keys, g_config_keys_size);
	init_console("Papito codeo en Assembler - Kernel v1.0", "kernel>", KERNEL);
	destroy_logger();
	return 0;
}
