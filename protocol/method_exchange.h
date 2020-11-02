#ifndef ACE_METHOD_EXCHANGE_H
#define ACE_METHOD_EXCHANGE_H

#include "misc/defs.h"

#define METHODS_LEN 257

typedef enum METHOD_T
{
    METHOD_NOAUTH = 0,
    METHOD_GSSAPI,
    METHOD_USERPASS ,
    METHOD_NOMETHOD = 0xFF,
} method_t;

bool SOCKS_exchange_methods(socket_t client);

#endif 
