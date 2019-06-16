#include "network.h"

int __recv(int socket, void* buffer, int bytes_to_read) {
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

int recv2(int socket, packet_t* packet) {
	int bytes_read;

	if ((bytes_read = __recv(socket, &packet->header, sizeof(header_t))) <= 0)
		return bytes_read; // que no lea basura si no trae bien la data

	if (packet->header.content_length > 0) {
		packet->content = realloc(packet->content, packet->header.content_length);
		bytes_read += __recv(socket, packet->content, packet->header.content_length);
	}

	return bytes_read;
}

int send2(int socket, packet_t* packet) {
	int bytes_sent;
	int length = packet->header.content_length;
	void* send_wrapper = malloc(sizeof(header_t) + length);

	memcpy(send_wrapper, &packet->header, sizeof(header_t));
	memcpy(send_wrapper + sizeof(header_t), packet->content, length);

	bytes_sent = send(socket, send_wrapper, sizeof(header_t) + length, 0);
	free(send_wrapper);

	return bytes_sent;
}

void build_packet(packet_t* packet, process_t process, operation_t operation, bool keep_alive, int length, void* content, bool success) {
	packet->header.process = process;
	packet->header.operation = operation;
	packet->header.keep_alive = keep_alive;
	packet->header.content_length = length;
	packet->content = malloc(length);
	packet->header.success = success;

	if (length > 0)
		memcpy(packet->content, content, length);
}

void free_packet(packet_t* packet) {
	if (packet->header.content_length > 0) {
		free(packet->content);
	}
	free(packet);
}
