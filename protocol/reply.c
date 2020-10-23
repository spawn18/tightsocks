#include "reply.h"
#include "network/io.h"

#include <string.h>


bool SOCKS_reply(fd_t client, rep_t rtyp, atyp_t atyp, const char* host, const char* port)
{
    char rep[REP_LEN + 1];
    int repLen = 4;

    rep[0] = SOCKS_VER;
    rep[1] = rtyp;
    rep[2] = 0;
    rep[3] = atyp;

    if(atyp == ATYP_IPV4)
    {
        memcpy(&rep[4], host, 4);
        memcpy(&rep[8], port, 2);
        repLen = 10;
    }
    else
    {
        memcpy(&rep[4], host, 16);
        memcpy(&rep[20],port, 2);
        repLen = 22;
    }

    return send_all(client, rep, repLen);
}