
#ifndef SOCKS_H
#define SOCKS_H

#include <sys/socket.h>

#include "types.h"

method_t exchange_methods(fd_t, method_t*);

void       request_connect(socks_request_t);
void          request_bind(socks_request_t);
void request_udp_associate(socks_request_t);
void     socks_get_request(fd_t, socks_request_t);

#endif