
#ifndef SOCKS_H
#define SOCKS_H

#include <sys/socket.h>

#include "types.h"

method_t exchange_methods(fd_t);
enum REPLY evaluate_request(fd_t);

#endif