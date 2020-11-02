#ifndef ACE_LOG_H
#define ACE_LOG_H


#include "misc/defs.h"
#include "protocol/request.h"

#define LOG_ENTRY_LEN 1024

typedef struct LOG_ENTRY_T
{
    char srcHost[HOST_LEN + 1];
    char srcPort[PORT_LEN + 1];
    char command[CMD_LEN + 1];
    char addrType[ATYP_LEN + 1];
    char dstHost[HOST_LEN + 1];
    char dstPort[PORT_LEN + 1];
} log_entry_t;


bool log_open();
void log_write(const log_entry_t*);
void log_fmt_entry(const struct sockaddr_storage *addr, const request_t*, log_entry_t*);

#endif 
