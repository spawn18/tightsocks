
#ifndef SOCKS_H
#define SOCKS_H

#include "misc/defs.h"

#include <sys/socket.h>

#define REQ_LEN 261

enum ATYP
{
    ATYP_IPV4 = 1,
    ATYP_DOMAINNAME = 3,
    ATYP_IPV6,
};

bool SOCKS_get_request(fd_t client, char* request);


#endif