#ifndef ACE_METHOD_EXCHANGE_H
#define ACE_METHOD_EXCHANGE_H

#include "misc/defs.h"


#define METHODS_LEN 257

enum METHODS
{
    METHOD_NOAUTH = 0,
    METHOD_USERPASS = 2,
    METHOD_NOMETHOD = 0xFF,
};


bool SOCKS_exchange_methods(fd_t client);

#endif 
