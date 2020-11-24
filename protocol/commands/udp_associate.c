#include "udp_associate.h"

#include "netinet/in.h"
#include "system/options.h"
#include "misc/utils.h"

#include <memory.h>
#include <stdlib.h>


void SOCKS_udp_associate(sock_t client, atyp_t atyp, char* host, char* port)
{
    char test[4] = {0};

    if(memcmp(host, test, 4) == 0 && memcmp(port, test, 2) == 0)
    {
        if(IS_OPT_SET(OPT_IP4))
        {
            sock_t dgramSock = socket(AF_INET, SOCK_DGRAM, 0);

            if(dgramSock != -1)
            {
                struct sockaddr_in addr4 = {0};
                addr4.sin_family = AF_INET;
                addr4.sin_addr.s_addr = INADDR_ANY;
                addr4.sin_port = htons(PORT);

                if(bind(dgramSock, (struct sockaddr*)&addr4, sizeof(addr4)) == 0)
                {

                }
            }
        }

        if(IS_OPT_SET(OPT_IP6))
        {
            sock_t dgramSock = socket(AF_INET6, SOCK_DGRAM, 0);

            if(dgramSock != -1)
            {
                struct sockaddr_in6 addr6 = {0};
                addr6.sin6_family = AF_INET6;
                addr6.sin6_addr = in6addr_any;
                addr6.sin6_port = htons(PORT);

                if(bind(dgramSock, (struct sockaddr*)&addr6, sizeof(addr6)) == 0)
                {

                }
            }
        }
    }
}
