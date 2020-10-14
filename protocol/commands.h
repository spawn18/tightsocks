#ifndef ACE_COMMANDS_H
#define ACE_COMMANDS_H

#include "misc/defs.h"
#include "request.h"


typedef enum CMD : char
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,

} cmd_t;



void SOCKS_bind(fd_t client, const char addrType, const char* host, const char* port);
void SOCKS_connect(fd_t client, const char addrType, const char* host, const char* port);
void SOCKS_udp_associate(fd_t client, const char addrType, const char* host, const char* port);

#endif 
