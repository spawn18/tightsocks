#ifndef HOMESOCKS_LOG_H
#define HOMESOCKS_LOG_H

#include "misc/defs.h"


typedef struct LOG_ENTRY_T
{
    char srcHost[255 + 1];
    char srcPort[255 + 1];
    char cmd[255 + 1];
    char addrType[255 + 1];
    char dstHost[255 + 1];
    char dstPort[255 + 1];
} log_entry_t;


bool log_open();
void log_write(const log_entry_t*);

#endif
