#include "request.h"

#include "protocol/methods/userpass.h"
#include "network/io.h"
#include "system/options.h"
#include "reply.h"

#include <string.h>

bool SOCKS_handle_request(fd_t client, request_t *req)
{
    char buf[REQUEST_LEN + 1];

    if(recv_all(client, buf, 4) > 0)
    {
        req->VER = buf[0];
        req->CMD = buf[1];
        req->RSV = buf[2];
        req->ATYP = buf[3];

        bool correctVER = req->VER == SOCKS_VER;
        bool correctRSV = req->RSV == 0x00;
        bool correctCMD  = ((req->CMD == CMD_CONNECT) && !is_decline_set(DECLINE_CONNECT)) ||
                           ((req->CMD == CMD_BIND)) && !is_decline_set(DECLINE_BIND) ||
                           ((req->CMD == CMD_UDP_ASSOCIATE) && !is_decline_set(DECLINE_UDP_ASSOCIATE));
        bool correctATYP = ((req->ATYP == ATYP_DOMAINNAME) && !is_decline_set(DECLINE_DOMAINNAME)) ||
                           ((req->ATYP == ATYP_IPV4) && !is_decline_set(DECLINE_IPV4)) ||
                           ((req->ATYP == ATYP_IPV6) && !is_decline_set(DECLINE_IPV6));

        reply_t reply = {SOCKS_VER,
                         REP_GENERAL_FAILURE,
                         0,
                         ATYP_IPV4,
                         0,
                         0};

        if(correctCMD)
        {
            if(correctATYP)
            {
                if(correctVER && correctRSV)
                {
                    if(req->ATYP == ATYP_DOMAINNAME)
                    {
                        recv_all(client, req->DSTADDR, 1);
                        recv_all(client, &req->DSTADDR[1], req->DSTADDR[0]);
                        recv_all(client, req->DSTPORT, 2);
                    }
                    else if(req->ATYP == ATYP_IPV4)
                    {
                        recv_all(client, req->DSTADDR, 4);
                        recv_all(client, req->DSTPORT, 2);
                    }
                    else if(req->ATYP == ATYP_IPV6)
                    {
                        recv_all(client, req->DSTADDR, 16);
                        recv_all(client, req->DSTPORT, 2);
                    }

                    return TRUE;
                }
            }
            else
            {
                reply.REP = REP_ADDRESS_TYPE_NOT_SUPPORTED;
            }
        }
        else
        {
            reply.REP = REP_COMMAND_NOT_SUPPORTED;
        }

        SOCKS_reply(client, &reply);
    }

    return FALSE;
}

