#include "request.h"

#include "protocol/methods/userpass.h"
#include "network/io.h"
#include "system/options.h"
#include "reply.h"

#include <string.h>
#include <system/firewall.h>

bool SOCKS_handle_request(socket_t client, request_t *req)
{
    char buf[BUFSIZE + 1];

    if(recv_all(client, buf, 5) > 0)
    {
        req->VER  = buf[0];
        req->CMD  = buf[1];
        req->RSV  = buf[2];
        req->ATYP = buf[3];

        reply_t reply = {SOCKS_VER,REP_GENERAL_FAILURE,0,ATYP_IPV4,0,0};

        if(req->VER == SOCKS_VER && req->RSV == 0)
        {
            if(req->ATYP == ATYP_DOMAINNAME || req->ATYP == ATYP_IPV4  || req->ATYP == ATYP_IPV6)
            {
                if(req->CMD == CMD_CONNECT || req->CMD == CMD_BIND || req->CMD == CMD_UDP_ASSOCIATE)
                {
                    if(req->ATYP == ATYP_DOMAINNAME)
                    {
                        recv_all(client, &buf[5], buf[4]+2);
                        memcpy(req->DSTADDR, &buf[5], buf[4]);
                        memcpy(req->DSTPORT, &buf[5+buf[4]], 2);
                    }
                    else if(req->ATYP == ATYP_IPV4)
                    {
                        recv_all(client, &buf[5], 5);
                        memcpy(req->DSTADDR, &buf[4], 4);
                        memcpy(req->DSTPORT, &buf[8], 2);
                    }
                    else if(req->ATYP == ATYP_IPV6)
                    {
                        recv_all(client, &buf[5], 17);
                        memcpy(req->DSTADDR, &buf[4], 16);
                        memcpy(req->DSTPORT, &buf[20], 2);
                    }

                    if(is_opt_set(OPT_FIREWALL))
                    {
                        fw_rule_t rule = {0};

                        strncpy(rule.host, &req->DSTADDR[1], req->DSTADDR[0]);
                        strncpy(rule.port, req->DSTPORT, 2);

                        if(fw_find(&rule))
                        {
                            reply.REP = REP_NOT_ALLOWED_BY_RULESET;
                            SOCKS_reply(client, &reply);
                            return FALSE;
                        }
                    }

                    return TRUE;
                }
                else
                {
                    reply.REP = REP_COMMAND_NOT_SUPPORTED;
                }
            }
            else
            {
                reply.REP = REP_ADDRESS_TYPE_NOT_SUPPORTED;
            }

            SOCKS_reply(client, &reply);
        }
    }

    return FALSE;
}

