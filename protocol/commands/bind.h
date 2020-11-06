#ifndef ACE_BIND_H
#define ACE_BIND_H

#include "protocol/request.h"
#include "misc/defs.h"


void SOCKS_bind(sock_t, const request_t*);

#endif 
