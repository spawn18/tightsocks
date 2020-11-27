#ifndef HOMESOCKS_REPLY_H
#define HOMESOCKS_REPLY_H

#include "misc/defs.h"
#include "request.h"

typedef enum REP_T
{
    REP_SUCCEEDED = 0,
    REP_GENERAL_FAILURE,
    REP_CONN_NOT_ALLOWED,
    REP_NET_UNREACH,
    REP_HOST_UNREACH,
    REP_CONN_REFUSED,
    REP_TTL_EXP,
    REP_CMD_NOT_SUPP,
    REP_ATYP_NOT_SUPP,
} rep_t;

typedef struct REPLY_T
{
    char VER;
    char REP;
    char RSV;
    char ATYP;
    char BNDADDR[255];
    char BNDPORT[2];
} reply_t;

bool SOCKS_reply(sock_t, reply_t*);
bool SOCKS_reply_fail(sock_t, rep_t);

#endif 
