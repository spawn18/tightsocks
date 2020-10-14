#ifndef ACE_UTILS_H
#define ACE_UTILS_H

#include "protocol/request.h"
#include "misc/defs.h"

#include <netinet/in.h>
#include <netdb.h>

void fmt_req(const char addrType,
             const char* hostIn,
             const char* portIn,
             char* hostOut,
             char* portOut);
void itos(int d, char* s);


#endif
