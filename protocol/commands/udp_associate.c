#include "udp_associate.h"

#include "netinet/in.h"
#include "system/options.h"
#include "misc/utils.h"
#include "protocol/reply.h"


void SOCKS_udp_associate(sock_t client, atyp_t atyp, char* host, char* port)
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if(memcmp(host, "0000", 4) == 0 && memcmp(port, "00", 2) == 0)
    {
        if(atyp == ATYP_IPV4)
        {
            addr.ss_family = AF_INET;
            ((struct sockaddr_in*)&addr)->sin_addr.s_addr = INADDR_ANY;
            ((struct sockaddr_in*)&addr)->sin_port = htons(PORT);
        }

        if(atyp == ATYP_IPV6)
        {
            addr.ss_family = AF_INET6;
            ((struct sockaddr_in6*)&addr)->sin6_addr = in6addr_any;
            ((struct sockaddr_in6*)&addr)->sin6_port = htons(PORT);
        }
    }
    else
    {
        if(atyp == ATYP_IPV4)
        {
            memcpy(&((struct sockaddr_in*)&addr)->sin_addr.s_addr, host, 4);
            memcpy(&((struct sockaddr_in*)&addr)->sin_port, port, 2);
        }
        if(atyp == ATYP_IPV6)
        {
            memcpy(&((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr, host, 4);
            memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, port, 2);
        }
    }

    sock_t dgramSock = socket(AF_INET, SOCK_DGRAM, 0);

    if(dgramSock != -1)
    {
        if(bind(dgramSock, (struct sockaddr*)&addr, sizeof(addr)) == 0)
        {
            struct sockaddr_storage bndAddr;
            socklen_t bndLen = sizeof(addr);

            if(getpeername(dgramSock, (struct sockaddr*)&bndAddr, &bndLen) == 0)
            {
                reply_t reply;
                reply.VER = SOCKS_VER;
                reply.RSV = 0;
                reply.REP = REP_SUCCEEDED;
                reply.ATYP = ATYP_IPV4;

                if(bndAddr.ss_family == AF_INET)
                {
                    memcpy(reply.BNDADDR, &((struct sockaddr_in*)&addr)->sin_addr.s_addr, 4);
                    memcpy(reply.BNDPORT, &((struct sockaddr_in*)&addr)->sin_port, 2);
                }
                else
                {
                    memcpy(reply.BNDADDR, &((struct sockaddr_in6*)&addr)->sin6_addr, 16);
                    memcpy(reply.BNDPORT, &((struct sockaddr_in6*)&addr)->sin6_port, 2);
                }

                SOCKS_reply(client, &reply);
                relay_dgrams(client, dgramSock);
                return;
            }
        }
    }

    SOCKS_reply_fail(client, REP_GENERAL_FAILURE);
}

// TODO: Read more about UDP socks rfc
// TODO: Why does incorrect NBO work
// TODO: Bind cmd
// TODO: daemon

void relay_dgrams(sock_t tcpSock, sock_t udpSock)
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    char buf[1500];
    recvfrom(udpSock, buf, 1500, 0, (struct sockaddr*)&addr, &len);
}