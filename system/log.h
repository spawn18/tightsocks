#ifndef ACE_LOG_H
#define ACE_LOG_H


#include "misc/defs.h"
#include "protocol/request.h"

#define LOG_ENTRY_LEN 1024
#define LOG_FIELD_LEN 255

typedef struct LOG_ENTRY_T
{
    char srcHost[LOG_FIELD_LEN + 1];
    char srcPort[LOG_FIELD_LEN + 1];
    char command[LOG_FIELD_LEN + 1];
    char addrType[LOG_FIELD_LEN + 1];
    char dstHost[LOG_FIELD_LEN + 1];
    char dstPort[LOG_FIELD_LEN + 1];
} log_entry_t;


bool log_open(char*);
void log_write(const log_entry_t*);
void log_fmt_entry(const struct sockaddr_storage *addr, const request_t*, log_entry_t*);

#endif 
