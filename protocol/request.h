
#ifndef SOCKS_H
#define SOCKS_H

#include "misc/defs.h"

#include <sys/socket.h>




bool SOCKS_get_request(fd_t client, char* request);


#endif