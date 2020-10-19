#ifndef ACE_COMMANDS_H
#define ACE_COMMANDS_H

#include "misc/defs.h"
#include "request.h"





void SOCKS_bind(fd_t, atyp_t, const char*, const char*);
void SOCKS_connect(fd_t, atyp_t, const char*, const char*);
void SOCKS_udp_associate(fd_t, atyp_t, const char*, const char*);

#endif 
