#include "request.h"

#include "protocol/methods/userpass.h"
#include "network/io.h"
#include "system/options.h"
#include "reply.h"

#include <string.h>
#include <stdio.h>


bool SOCKS_get_request(sock_t client, request_t *req)
{
    char buf[BUFSIZE + 1];

    if (recv_all(client, buf, 5) < 0)
    {
        req->VER = buf[0];
        req->CMD = buf[1];
        req->RSV = buf[2];
        req->ATYP = buf[3];

        if (req->VER == SOCKS_VER && req->RSV == 0)
        {
            if (req->CMD == CMD_CONNECT || req->CMD == CMD_BIND || req->CMD == CMD_UDP_ASSOCIATE)
            {
                if (req->ATYP == ATYP_IPV4)
                {
                    recv_all(client, &buf[5], 5);
                    memcpy(req->DSTADDR, &buf[4], 4);
                    memcpy(req->DSTPORT, &buf[8], 2);
                }
                else if (req->ATYP == ATYP_IPV6)
                {
                    recv_all(client, &buf[5], 17);
                    memcpy(req->DSTADDR, &buf[4], 16);
                    memcpy(req->DSTPORT, &buf[20], 2);
                }
                else
                {
                    recv_all(client, &buf[5], buf[4] + 2);
                    memcpy(req->DSTADDR, &buf[5], buf[4]);
                    memcpy(req->DSTPORT, &buf[5 + buf[4]], 2);
                }

                return TRUE;
            }
        }
    }

    reply_t reply;

    reply.VER = SOCKS_VER;
    reply.REP = REP_GENERAL_FAILURE;
    reply.RSV = 0;
    reply.ATYP = 0;
    memset(reply.BNDADDR, 0, 255);
    memset(reply.BNDPORT, 0, 2);

    SOCKS_reply(client, &reply);
    return FALSE;
}
