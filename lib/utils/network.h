#ifndef UTILS_NETWORK_H_
#define UTILS_NETWORK_H_

#include <sys/socket.h>

int recv2(int socket, void* buffer, int bytes_to_read);

#endif /* UTILS_NETWORK_H_ */
