#ifndef SOCKS5_SERVER_CONVERT_H
#define SOCKS5_SERVER_CONVERT_H

#include <netinet/in.h>

#include "types.h"

void* getportss(struct sockaddr_storage* ss);

void* getaddrss(struct sockaddr_storage* ss);

void fill_ss(struct sockaddr_storage*, ADDRESS_TYPE, address_t, port_t)

#endif //SOCKS5_SERVER_CONVERT_H
