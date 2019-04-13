#include "operation_types.h"

char* get_process_name(process_t process) {
	switch (process) {
		case KERNEL: return "Kernel";
		case FILESYSTEM: return "Filesystem";
		case MEMORY: return "Memoria";
		default: return "Desconocido";
	}
}
