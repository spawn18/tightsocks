
#ifndef SOCKS_H
#define SOCKS_H

#include "misc/defs.h"

#include <sys/socket.h>

#define REQUEST_LEN 261

typedef struct REQUEST_T
{
    char    VER;
    cmd_t   CMD;
    char    RSV;
    atyp_t  ATYP;
    char    DSTADDR[HOST_LEN + 1];
    char    DSTPORT[PORT_LEN + 1];
} request_t;


bool SOCKS_handle_request(socket_t, request_t*);


#endif