#ifndef ACE_REPLY_H
#define ACE_REPLY_H

#include "misc/defs.h"
#include <netdb.h>

enum REP
{
    REP_SUCCEEDED = 0,
    REP_GENERAL_ERROR,
};



bool SOCKS_reply(fd_t client,
                 const char replyType,
                 const char addrType,
                 const char* host,
                 const char* port);

#endif 
