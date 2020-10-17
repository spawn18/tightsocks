#ifndef ACE_COMMANDS_H
#define ACE_COMMANDS_H

#include "misc/defs.h"
#include "request.h"


typedef enum CMD
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,

} cmd_t;



void SOCKS_bind(fd_t, char, const char*, const char*);
void SOCKS_connect(fd_t, char, const char*, const char*);
void SOCKS_udp_associate(fd_t, char, const char*, const char*);

#endif 
