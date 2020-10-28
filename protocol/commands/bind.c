#include "bind.h"

#include "misc/utils.h"

void SOCKS_bind(fd_t client, char* req)
{
    atyp_t atyp;
    char* host;
    char* port;

    parse_req(req, &atyp, &host, &port);
}
