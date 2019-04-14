#include "network.h"

int recv2(int socket, void* buffer, int bytes_to_read) {
	int bytes_read = 0;
	int current_bytes_read;

	while (bytes_to_read > 0) {
		current_bytes_read = recv(socket, buffer + bytes_read, bytes_to_read, 0);

		if (current_bytes_read <= 0) // something is wrong, or nothing was sent
			return current_bytes_read;

		bytes_to_read -= current_bytes_read;
		bytes_read += current_bytes_read;
	}

	return bytes_read;
}
