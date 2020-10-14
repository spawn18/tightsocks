#ifndef ACE_MESSAGE_H
#define ACE_MESSAGE_H

#include "misc/defs.h"


#define MSG(msg) \
        if(!is_opt_set(OPT_DAEMON)) \
        { \
            printf(msg); \
        } \

#define MAX_LOG_NAME_LEN 100
#define MAX_LOG_RECORD_LEN 1024
#define MAX_CHRONO_LEN 25


void log_write(const char* host, const char* port, const char* cmd, const char* dstHost, const char* dstPort);

/* Open new log file */
bool log_open();

/* Check at any given time if log file is open */
bool is_log_open();

/* Save log file and close it */
void log_save();
void log_close();

#endif
