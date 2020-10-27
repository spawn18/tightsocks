#include "request.h"

#include "protocol/methods/userpass.h"
#include "network/io.h"
#include "system/options.h"
#include "reply.h"


bool SOCKS_handle_request(fd_t client, char* req)
{
    if(recv_all(client, req, 5) > 0)
    {
        bool correctVer  = req[0] == SOCKS_VER;
        bool correctRsv = req[2] == 0x00;

        bool correctCmd  = ((req[1] == CMD_CONNECT) && !is_decline_set(DECLINE_CONNECT)) ||
                           ((req[1] == CMD_BIND)) && !is_decline_set(DECLINE_BIND) ||
                           ((req[1] == CMD_UDP_ASSOCIATE) && !is_decline_set(DECLINE_UDP_ASSOCIATE));

        bool correctAtyp = ((req[3] == ATYP_DOMAINNAME) && !is_decline_set(DECLINE_DOMAINNAME)) ||
                           ((req[3] == ATYP_IPV4) && !is_decline_set(DECLINE_IPV4)) ||
                           ((req[3] == ATYP_IPV6) && !is_decline_set(DECLINE_IPV6));

        if(correctCmd)
        {
            if(correctAtyp)
            {
                if(correctVer && correctRsv)
                {
                    int rem = 0;
                    int lenPort = 2;

                    if      (req[3] == ATYP_DOMAINNAME) rem = (unsigned char)req[4];
                    else if (req[3] == ATYP_IPV4)       rem = 3;
                    else if (req[3] == ATYP_IPV6)       rem = 15;

                    recv_all(client, req+5, rem+lenPort);
                    return TRUE;
                }
                else SOCKS_reply(client, REP_GENERAL_FAILURE, 0, "0000", "00");
            }
            else SOCKS_reply(client, REP_ADDRESS_TYPE_NOT_SUPPORTED, 0, "0000", "00");
        }
        else SOCKS_reply(client, REP_COMMAND_NOT_SUPPORTED, 0, "0000", "00");
    }

    return FALSE;
}

