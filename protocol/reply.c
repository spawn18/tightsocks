#include "reply.h"
#include "network/io.h"

#include <string.h>


bool SOCKS_reply_fail(sock_t client, rep_t REP)
{
    reply_t reply;

    reply.VER = SOCKS_VER;
    reply.REP = REP;
    reply.RSV = 0;
    reply.ATYP = 0;
    memset(reply.BNDADDR, 0, 255);
    memset(reply.BNDPORT, 0, 2);

    return SOCKS_reply(client, &reply);
}

bool SOCKS_reply(sock_t client, reply_t *reply)
{
    char buf[261] = {0};
    int len = 4;

    buf[0] = reply->VER;
    buf[1] = reply->REP;
    buf[2] = reply->RSV;
    buf[3] = reply->ATYP;

    if(reply->ATYP == ATYP_IPV4)
    {
        memcpy(&buf[4], reply->BNDADDR, 4);
        memcpy(&buf[8], reply->BNDPORT, 2);
        len += 6;
    }
    else
    {
        memcpy(&buf[4], reply->BNDADDR, 16);
        memcpy(&buf[20], reply->BNDPORT, 2);
        len += 18;
    }

    return send_all(client, buf, len);
}