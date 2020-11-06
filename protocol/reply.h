#ifndef ACE_REPLY_H
#define ACE_REPLY_H

#include "misc/defs.h"
#include "request.h"

typedef enum REP_T
{
    REP_SUCCEEDED = 0,
    REP_GENERAL_FAILURE,
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

bool SOCKS_reply(sock_t, reply_t*);

#endif 
