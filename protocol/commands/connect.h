#ifndef ACE_CONNECT_H
#define ACE_CONNECT_H

#include "misc/defs.h"
#include "protocol/request.h"

void SOCKS_connect(socket_t, const request_t*);

#endif 