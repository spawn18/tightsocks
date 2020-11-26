#ifndef HOMESOCKS_UDP_ASSOCIATE_H
#define HOMESOCKS_UDP_ASSOCIATE_H

#include "protocol/request.h"
#include "misc/defs.h"

void SOCKS_udp_associate(sock_t, atyp_t, char *, char *);
void relay_dgrams(sock_t, sock_t);

typedef struct UDP_REQUEST_T
{
    char RSV[2];
    char FRAG;
    char ATYP;
    char DSTADDR[255];
    char DSTPORT[2];
} udp_request_t;
#endif
