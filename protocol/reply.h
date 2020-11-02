#ifndef ACE_REPLY_H
#define ACE_REPLY_H

#include "misc/defs.h"
#include "request.h"

#include <netdb.h>

#define REPLY_LEN 261

typedef struct REPLY_T
{
    char    VER;
    rep_t   REP;
    char    RSV;
    atyp_t  ATYP;
    char    BNDADDR[HOST_LEN + 1];
    char    BNDPORT[PORT_LEN + 1];
} reply_t;


bool SOCKS_reply(socket_t, reply_t*);

#endif 
