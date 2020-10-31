#ifndef ACE_REPLY_H
#define ACE_REPLY_H

#include "misc/defs.h"
#include "request.h"

#include <netdb.h>



bool SOCKS_reply(fd_t, reply_t*);

#endif 
