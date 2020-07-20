
#ifndef SOCKS_H
#define SOCKS_H

#include <sys/socket.h>

#include "types.h"

method_t exchange_methods(fd_t);

enum REPLY wait_for_request(fd_t);

void socks_reply(fd_t, enum REPLY, enum ADDRTYPE, const_address_t, const_port_t);

inline index_t socks_get_address_size(const char*);

inline index_t socks_get_port_index(const char*);

void request_connect(ADDRESS_TYPE, size_t, address_t, port_t);
void request_bind(ADDRESS_TYPE, size_t, address_t, port_t);
void request_udp_associate(ADDRESS_TYPE, size_t, address_t, port_t);

void socks_get_request(fd_t, socks_request_t*);

#endif