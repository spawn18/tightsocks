#ifndef ACE_LOG_H
#define ACE_LOG_H

#include <sys/socket.h>
#include "misc/defs.h"

bool log_open();
void log_write(log_entry_t);
void log_fmt_entry(struct sockaddr_storage addr, char* req, log_entry_t* entry);

#endif 
