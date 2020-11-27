#include "udp_associate.h"

#include "netinet/in.h"
#include "system/options.h"
#include "misc/utils.h"
#include "protocol/reply.h"


void SOCKS_udp_associate(sock_t client, atyp_t atyp, char* host, char* port)
{
    // unsupported
}

// TODO: Why does incorrect NBO work
// TODO: daemon
