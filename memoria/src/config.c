#include "config.h"

void initialize_mem_config(t_config* config) {
	g_config.port = config_get_int_value(config, "PUERTO");

	char* fs_ip = config_get_string_value(config, "IP_FS");
	g_config.fs_ip = malloc(strlen(fs_ip));
	strcpy(g_config.fs_ip, fs_ip);

	g_config.fs_port = config_get_int_value(config, "PUERTO_FS");

	char** ip_seeds = config_get_array_value(config,"IP_SEEDS");
	g_config.ip_seeds = malloc(sizeof(ip_seeds));
	g_config.ip_seeds = config_get_array_value(config,"IP_SEEDS");

	int** ports_seeds = config_get_array_value(config,"PUERTO_SEEDS");
	g_config.ports_seeds = malloc(sizeof(ports_seeds));
	g_config.ports_seeds = config_get_array_value(config,"PUERTO_SEEDS");

	g_config.memory_ret = config_get_int_value(config, "RETARDO_MEM");
	g_config.fs_ret = config_get_int_value(config, "RETARDO_FS");
	g_config.memory_size = config_get_int_value(config, "TAM_MEM");
	g_config.journal_ret = config_get_int_value(config, "RETARDO_JOURNAL");
	g_config.gossiping_ret = config_get_int_value(config, "RETARDO_GOSSIPING");
	g_config.memory_number = config_get_int_value(config, "MEMORY_NUMBER");
}

void update_mem_config(t_config* config) {
	int new_memory_ret = config_get_int_value(config, "RETARDO_MEM");
	int new_fs_ret = config_get_int_value(config, "RETARDO_FS");
	int new_memory_size = config_get_int_value(config, "TAM_MEM");
	int new_journal_ret = config_get_int_value(config, "RETARDO_JOURNAL");
	int new_gossiping_ret = config_get_int_value(config, "RETARDO_GOSSIPING");
	int new_memory_number = config_get_int_value(config, "MEMORY_NUMBER");

	if (new_memory_ret != g_config.memory_ret)
		log_info(g_logger, "Nuevo valor de RETARDO_MEM detectado -> %i", g_config.memory_ret = new_memory_ret);
	if (new_fs_ret != g_config.fs_ret)
		log_info(g_logger, "Nuevo valor de RETARDO_FS detectado -> %i", g_config.fs_ret = new_fs_ret);
	if (new_memory_size != g_config.memory_size)
		log_info(g_logger, "Nuevo valor de TAM_MEM detectado -> %i", g_config.memory_size = new_memory_size);
	if (new_journal_ret!= g_config.journal_ret)
			log_info(g_logger, "Nuevo valor de RETARDO_JOURNAL detectado -> %i", g_config.journal_ret = new_journal_ret);
	if (new_gossiping_ret != g_config.gossiping_ret)
			log_info(g_logger, "Nuevo valor de RETARDO_GOSSIPING detectado -> %i", g_config.gossiping_ret = new_gossiping_ret);
	if (new_memory_number != g_config.memory_number)
			log_info(g_logger, "Nuevo valor de MEMORY_NUMBER detectado -> %i", g_config.memory_number = new_memory_number);
}

