#ifndef ACE_LOG_H
#define ACE_LOG_H

#include <sys/socket.h>
#include "misc/defs.h"

bool log_open();
void log_write(const log_entry_t*);
void log_fmt_entry(const struct sockaddr_storage *addr, const request_t*, log_entry_t*);

#endif 
