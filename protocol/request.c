#include "request.h"

#include "misc/utils.h"
#include "protocol/methods/userpass.h"
#include "network/io.h"
#include "system/log.h"
#include "protocol/commands.h"
#include "system/options.h"
#include "reply.h"


bool SOCKS_get_request(fd_t client, char* req)
{
    recv_all(client, req, 5);

    bool version  = req[0] == SOCKS_VER;
    bool command  = (req[1] == CMD_CONNECT) || (req[1] == CMD_BIND) || ((req[1] == CMD_UDP_ASSOCIATE));
    bool reserved = req[2] == 0x00;
    bool addrType = (req[3] == ATYP_DOMAINNAME) || (req[3] == ATYP_IPV4) || (req[3] == ATYP_IPV6);


    if(version && command && reserved && addrType)
    {
        int rem = 0;
        int lenPort = 2;

        if      (req[3] == ATYP_DOMAINNAME) rem = req[4];
        else if (req[3] == ATYP_IPV4)       rem = 3;
        else if (req[3] == ATYP_IPV6)       rem = 15;

        recv_all(client, req+5, rem+lenPort);
        return TRUE;
    }
    else
    {
        SOCKS_reply(client, REP_GENERAL_ERROR, 0, 0, 0);
        return FALSE;
    }
}

