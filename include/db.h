
#ifndef DB_H
#define DB_H

#include "types.h"

typedef struct socks_client
{
    fd_t client;
    method_t method;
    COMMAND command;
} socks_client;


#endif
