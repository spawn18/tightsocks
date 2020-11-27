#include "request.h"

#include "network/io.h"
#include "system/options.h"
#include "reply.h"

#include <string.h>


bool SOCKS_get_request(sock_t client, request_t *req)
{
    char buf[261];

    if (recv_all(client, buf, 5) < 0) return FALSE;

    req->VER = buf[0];
    req->CMD = buf[1];
    req->RSV = buf[2];
    req->ATYP = buf[3];

    if (req->VER == SOCKS_VER && req->RSV == 0)
    {
        if (0 < req->CMD && req->CMD < 4)
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
            else if(req->ATYP == ATYP_DOMAINNAME)
            {
                recv_all(client, &buf[5], buf[4] + 2);
                memcpy(req->DSTADDR, &buf[4], buf[4]+1);
                memcpy(req->DSTPORT, &buf[5 + buf[4]], 2);
            }
            else
            {
                SOCKS_reply_fail(client, REP_ATYP_NOT_SUPP);
                return FALSE;
            }

            return TRUE;
        }
        else
        {
            SOCKS_reply_fail(client, REP_CMD_NOT_SUPP);
        }
    }
    else
    {
        SOCKS_reply_fail(client, REP_GENERAL_FAILURE);
    }

    return FALSE;
}


