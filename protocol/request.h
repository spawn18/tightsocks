
#ifndef SOCKS_H
#define SOCKS_H

#include "misc/defs.h"

#include <sys/socket.h>




bool SOCKS_handle_request(fd_t, request_t*);


#endif