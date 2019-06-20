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
	int bytes_read = 0;
	int partial_header_size = sizeof(process_t) + sizeof(socket_operation_t) + sizeof(bool) + sizeof(int);
	char* partial_buffer = malloc(partial_header_size);
	int offset = 0;

	// Primero, hay que traer las cosas del header
	if ((bytes_read += __recv(socket, partial_buffer, partial_header_size)) <= 0)
		return bytes_read; // que no lea basura si no trae bien la data

	// Tenemos los primeros cuatro parametros del header, el cual tenemos que armar ahora.
	packet->header.process = (process_t) *partial_buffer;
	offset += sizeof(process_t);
	packet->header.operation = (socket_operation_t) *(partial_buffer + offset);
	offset += sizeof(socket_operation_t);
	packet->header.keep_alive = *(partial_buffer + offset);
	offset += sizeof(bool);
	packet->header.elements = *(partial_buffer + offset);
	offset += sizeof(int);

	free(partial_buffer);
	offset = 0;

	// Sigo con los sizes y bool
	partial_buffer = malloc(packet->header.elements * sizeof(int) + sizeof(bool));
	bytes_read += __recv(socket, partial_buffer, packet->header.elements * sizeof(int) + sizeof(bool));

	if (packet->header.elements == 0) {
		packet->header.elements_size = NULL;
	} else {
		packet->header.elements_size = malloc(packet->header.elements * sizeof(int));
		memcpy(packet->header.elements_size, partial_buffer, packet->header.elements * sizeof(int));
		offset += packet->header.elements * sizeof(int);
	}

	// Y Terminamos de armar el header
	packet->header.success = *(partial_buffer + offset);
	offset += sizeof(bool);

	free(partial_buffer);

	if (packet->header.elements > 0) {
		int total_content_length = 0;
		for (int i = 0; i < packet->header.elements; i++)
			total_content_length += packet->header.elements_size[i];

		if (packet->content != NULL)
			free(packet->content);
		packet->content = malloc(total_content_length);
		bytes_read += __recv(socket, packet->content, total_content_length);
	}

	return bytes_read;
}

int send2(int socket, packet_t* packet) {
	int bytes_sent;
	int content_length = 0;
	int offset = 0;

	for (int i = 0; i < packet->header.elements; i++)
		content_length += packet->header.elements_size[i];

	// como se envia lo serializado?
	// [process|operation|keep_alive|elements|elements_size|success][content]
	int wrapper_length = sizeof(process_t) + sizeof(socket_operation_t) + sizeof(bool) + sizeof(int) +
			packet->header.elements * sizeof(int) + sizeof(bool) + content_length;
	void* send_wrapper = malloc(wrapper_length);

	memcpy(send_wrapper + offset, &(packet->header.process), sizeof(process_t));
	offset += sizeof(process_t);
	memcpy(send_wrapper + offset, &(packet->header.operation), sizeof(socket_operation_t));
	offset += sizeof(socket_operation_t);
	memcpy(send_wrapper + offset, &(packet->header.keep_alive), sizeof(bool));
	offset += sizeof(bool);
	memcpy(send_wrapper + offset, &(packet->header.elements), sizeof(int));
	offset += sizeof(int);
	memcpy(send_wrapper + offset, packet->header.elements_size, packet->header.elements * sizeof(int));
	offset += packet->header.elements * sizeof(int);
	memcpy(send_wrapper + offset, &(packet->header.success), sizeof(bool));
	offset += sizeof(bool);
	memcpy(send_wrapper + offset, packet->content, content_length);

	bytes_sent = send(socket, send_wrapper, wrapper_length, 0);

	free(send_wrapper);
	free_packet(packet);
	return bytes_sent;
}

void build_packet(packet_t* packet, process_t process, socket_operation_t operation, bool keep_alive, int elements, int* elements_size, void* content, bool success) {
	int content_length = 0;
	int* iterator = elements_size;
	for (int i = 0; i < elements; i++) {
		content_length += *iterator;
		iterator++;
	}

	packet->header.process = process;
	packet->header.operation = operation;
	packet->header.keep_alive = keep_alive;
	packet->header.elements = elements;
	packet->header.elements_size = elements_size;
	packet->header.success = success;
	if (content_length > 0) {
		packet->content = malloc(content_length);
	} else {
		packet->content = NULL;
	}

	serialize_content(packet->content, operation, content);
}

void free_packet(packet_t* packet) {
	if (packet->header.elements > 0) {
		free(packet->header.elements_size);
		free(packet->content);
	}
	free(packet);
}
