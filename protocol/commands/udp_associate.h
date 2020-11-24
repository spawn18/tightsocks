#ifndef HOMESOCKS_UDP_ASSOCIATE_H
#define HOMESOCKS_UDP_ASSOCIATE_H

#include "protocol/request.h"
#include "misc/defs.h"

void SOCKS_udp_associate(sock_t, atyp_t, char *, char *);
void relay_dgrams(sock_t, sock_t);

#endif 
