#ifndef HOMESOCKS_METHOD_EXCHANGE_H
#define HOMESOCKS_METHOD_EXCHANGE_H

#include "misc/defs.h"

typedef enum METHOD_T
{
    MET_NOAUTH = 0,
    MET_GSSAPI = 1,
    MET_USERPASS = 2,
    MET_NOMETHOD = 0xFF,
} method_t;

bool SOCKS_handle_method(sock_t client);
bool auth_userpass(sock_t client);

#endif 
