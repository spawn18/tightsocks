#ifndef HOMESOCKS_UDP_ASSOCIATE_H
#define HOMESOCKS_UDP_ASSOCIATE_H

#include "protocol/request.h"

void SOCKS_udp_associate(sock_t, atyp_t, char*, char*);

typedef struct UDP_DGRAM_T
{
	char RSV[2];
	char FRAG;
	char ATYP;
	char DSTADDR[255];
	char DSTPORT[2];
	char DATA[65535];
};

#endif
