
#ifndef HOMESOCKS_REQUEST_H
#define HOMESOCKS_REQUEST_H

#include "misc/defs.h"

#include <sys/socket.h>

typedef enum CMD_T
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,
} cmd_t;

typedef enum ATYP_T
{
    ATYP_IPV4 = 1,
    ATYP_DOMAINNAME = 3,
    ATYP_IPV6,
} atyp_t;

typedef struct REQUEST_T
{
    char    VER;
    cmd_t   CMD;
    char    RSV;
    atyp_t  ATYP;
    char    DSTADDR[255];
    char    DSTPORT[2];
} request_t;


bool SOCKS_get_request(sock_t, request_t*);


#endif