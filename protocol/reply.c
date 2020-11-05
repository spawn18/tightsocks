#include "reply.h"
#include "network/io.h"

#include <string.h>


bool SOCKS_reply(socket_t client, reply_t *reply)
{
    char buf[BUFSIZE + 1] = {0};

    buf[0] = reply->VER;
    buf[1] = reply->REP;
    buf[2] = reply->RSV;
    buf[3] = reply->ATYP;

    if(reply->ATYP == ATYP_IPV4)
    {
        memcpy(&buf[4], reply->BNDADDR, 4);
        memcpy(&buf[8], reply->BNDPORT, 2);
    }
    else
    {
        memcpy(&buf[4], reply->BNDADDR, 16);
        memcpy(&buf[20], reply->BNDPORT, 2);
    }

    return send_all(client, buf, strlen(buf));
}