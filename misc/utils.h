#ifndef ACE_UTILS_H
#define ACE_UTILS_H

#include "protocol/request.h"
#include "misc/defs.h"

#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

void addr_to_str(const struct sockaddr_storage *addr, char* host, char* port);
void req_addr_to_str(atyp_t, const char *, const char *, char *, char *);

static inline void cmd_to_str(cmd_t cmd, char *str)
{
    if      (cmd == CMD_CONNECT) strcpy(str, "C");
    else if (cmd == CMD_BIND)    strcpy(str, "B");
    else                         strcpy(str, "U");
}

static inline void atyp_to_str(atyp_t atyp, char *str)
{
    if      (atyp == ATYP_IPV4) strcpy(str, "4");
    else if (atyp == ATYP_IPV6) strcpy(str, "6");
    else                        strcpy(str, "D");

}
#endif