
#ifndef ACE_IO_H
#define ACE_IO_H

#include "misc/defs.h"

#include <stddef.h>

bool recv_all(socket_t, char*, size_t);
bool send_all(socket_t, const char*, size_t);
void handle_io(socket_t, socket_t);

#endif
