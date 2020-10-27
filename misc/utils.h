#ifndef ACE_UTILS_H
#define ACE_UTILS_H

#include "protocol/request.h"
#include "misc/defs.h"

#include <netinet/in.h>
#include <netdb.h>


void fmt_addr(struct sockaddr_storage addr, char* host, char* port);


static inline void parse_req(const char* req, atyp_t* atyp, const char* host, const char* port)
{
    *atyp = (unsigned char)req[3];
    host = &req[4];
    if(req[3] == ATYP_IPV4)
    {
        port = &req[8];
    }
    else if(req[3] == ATYP_IPV6)
    {
        port = &req[20];
    }
    else
    {
        port = &req[3+req[4]];
    }
}


#endif