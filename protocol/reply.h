#ifndef ACE_REPLY_H
#define ACE_REPLY_H

#include "misc/defs.h"
#include "request.h"

#include <netdb.h>

typedef enum REP_T
{
    REP_SUCCEEDED = 0,
    REP_GENERAL_FAILURE,
    REP_NOT_ALLOWED_BY_RULESET,
    REP_NETWORK_UNREACHABLE,
    REP_HOST_UNREACHABLE,
    REP_CONNECTION_REFUSED,
    REP_TTL_EXPIRED,
    REP_COMMAND_NOT_SUPPORTED,
    REP_ADDRESS_TYPE_NOT_SUPPORTED,
} rep_t;

typedef struct REPLY_T
{
    char    VER;
    rep_t   REP;
    char    RSV;
    atyp_t  ATYP;
    char    BNDADDR[255];
    char    BNDPORT[2];
} reply_t;



bool SOCKS_reply(socket_t, reply_t*);

#endif 
