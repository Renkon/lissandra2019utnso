#include "config.h"

void initialize_memory_config(t_config* config) {
	g_config.port = init_int_config_value("PUERTO", config);
	g_config.filesystem_ip = init_str_config_value("IP_FS", config);
	g_config.filesystem_port = init_int_config_value("PUERTO_FS", config);
	g_config.seed_ips = init_str_array_config_value("IP_SEEDS", config);
	g_config.seed_ports = init_int_array_config_value("PUERTO_SEEDS", config);
	g_config.memory_delay = init_int_config_value("RETARDO_MEM", config);
	g_config.filesystem_delay = init_int_config_value("RETARDO_FS", config);
	g_config.memory_size = init_int_config_value("TAM_MEM", config);
	g_config.journal_delay = init_int_config_value("RETARDO_JOURNAL", config);
	g_config.gossip_delay = init_int_config_value("RETARDO_GOSSIPING", config);
	g_config.memory_number = init_int_config_value("MEMORY_NUMBER", config);
}

void update_memory_config(t_config* config) {
	g_config.memory_delay = update_int_config_value(g_config.memory_delay, "RETARDO_MEM", config);
	g_config.filesystem_delay = update_int_config_value(g_config.filesystem_delay, "RETARDO_FS", config);
	g_config.journal_delay = update_int_config_value(g_config.journal_delay, "RETARDO_JOURNAL", config);
	g_config.gossip_delay = update_int_config_value(g_config.gossip_delay, "RETARDO_GOSSIPING", config);
}

