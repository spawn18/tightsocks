
#ifndef ACE_IO_H
#define ACE_IO_H

#include "misc/defs.h"

#include <stddef.h>

bool recv_all(fd_t, char*, size_t);
bool send_all(fd_t, const char*, size_t);
void handle_io(fd_t, fd_t);

#endif
