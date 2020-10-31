#ifndef ACE_UTILS_H
#define ACE_UTILS_H

#include "protocol/request.h"
#include "misc/defs.h"

#include <netinet/in.h>
#include <netdb.h>


void unload_addr(const struct sockaddr_storage *addr, char* host, char* port);


#endif