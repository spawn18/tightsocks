//
// Created by autumn on 29.10.2019.
//

#ifndef SOCKS5_SERVER_IO_H
#define SOCKS5_SERVER_IO_H

#include "server/include/types.h"

res_t recvf(fd_t sock, char* buf, size_t len);

res_t sendf(fd_t sock, const void* buf, size_t len);

#endif //SOCKS5_SERVER_IO_H
