
#ifndef ACE_IO_H
#define ACE_IO_H

#include "misc/defs.h"

#include <stddef.h>

void handle_data(fd_t, fd_t);
bool recv_all(fd_t sock, char* buf, size_t len);
bool send_all(fd_t sock, const char* buf, size_t len);

#endif
