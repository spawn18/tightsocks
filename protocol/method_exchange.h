#ifndef ACE_METHOD_EXCHANGE_H
#define ACE_METHOD_EXCHANGE_H

#include "misc/defs.h"

typedef enum METHOD_T
{
    MET_NOAUTH = 0,
    MET_GSSAPI,
    MET_USERPASS ,
    MET_NOMETHOD = 0xFF,
} method_t;


bool SOCKS_handle_method(socket_t client);

#endif 
