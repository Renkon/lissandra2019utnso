#include "operation_types.h"

char* get_process_name(process_t process) {
	switch (process) {
		case KERNEL: return "Kernel";
		case FILESYSTEM: return "Filesystem";
		case MEMORY: return "Memoria";
		default: return "Desconocido";
	}
}

char* get_consistency_name( consistency_t consistency) {
		switch (consistency) {
			case STRONG_CONSISTENCY: return "Strong Consistency";
			case STRONG_HASH_CONSISTENCY: return "Strong Hash Consistency";
			case EVENTUAL_CONSISTENCY: return "Eventual  Consistency";
			default: return "Desconocido";
		}

}


