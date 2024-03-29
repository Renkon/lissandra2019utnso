#include "kernel.h"

// Las keys de configuracion... pero ahora del kernel
char* g_config_keys[] = { "IP_MEMORIA", "PUERTO_MEMORIA", "QUANTUM", "MULTIPROCESAMIENTO", "METADATA_REFRESH",
		"SLEEP_EJECUCION", "LTS_DELAY", "METRICS_DISPLAY", "GOSSIP_REFRESH" };
int g_config_keys_size = 9;

int main(void) {
	if (!init_logger("kernel.log", "Kernel", true, LOG_LEVEL_TRACE))
		return 1;
	init_config(KNCFG, initialize_kernel_config, update_kernel_config, g_config_keys, g_config_keys_size);
	init_scheduler();
	init_memory_list();
	init_describer();
	init_gossiping();
	init_metrics();
	init_console("Papito codeo en Assembler - Kernel v1.0", "kernel>", KERNEL, get_callbacks());
	destroy_logger();
	return 0;
}

callbacks_t* get_callbacks() {
	return get_input_callbacks(process_select, process_insert, process_create, process_describe,
			process_drop, process_journal, process_add, process_run, process_metrics);
}
