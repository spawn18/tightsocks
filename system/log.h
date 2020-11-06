#ifndef ACE_LOG_H
#define ACE_LOG_H

#include "misc/defs.h"
#include "protocol/request.h"


typedef struct LOG_ENTRY_T
{
    char srcHost[255 + 1];
    char srcPort[255 + 1];
    char command[255 + 1];
    char addrType[255 + 1];
    char dstHost[255 + 1];
    char dstPort[255 + 1];
} log_entry_t;


bool log_open(char*);
void log_write(const log_entry_t*);
void log_fmt_entry(const struct sockaddr_storage *addr, const request_t*, log_entry_t*);

#endif 
