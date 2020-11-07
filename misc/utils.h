#ifndef ACE_UTILS_H
#define ACE_UTILS_H

#include "protocol/request.h"
#include "misc/defs.h"

#include <netinet/in.h>
#include <netdb.h>


void addr_to_str(const struct sockaddr_storage *addr, char* host, char* port);
void req_addr_to_str(atyp_t, char *, char *, char *, char *);
void cmd_to_str(cmd_t, char *);
void atyp_to_str(atyp_t, char *);

#endif