#include "commands.h"
#include "system/options.h"
#include "system/log.h"
#include "misc/utils.h"
#include "protocol/reply.h"
#include "misc/defs.h"
#include "misc/wrappers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>


void SOCKS_connect(fd_t client, const char atyp, const char* host, const char* port)
{
    fd_t dest;

    bool done = FALSE;
    if(atyp == ATYP_DOMAINNAME)
    {
        struct addrinfo gaiInfo;
        struct addrinfo* res;

        memset(&gaiInfo, 0, sizeof(struct addrinfo));

        gaiInfo.ai_family    = AF_INET | AF_INET6;
        gaiInfo.ai_socktype  = SOCK_STREAM;
        gaiInfo.ai_protocol  = 0;
        gaiInfo.ai_flags     = 0;

        if(getaddrinfo(host, port, &gaiInfo, &res) == 0)
        {
            struct addrinfo* p_res = NULL;
            for(p_res = res; p_res != NULL; p_res = p_res->ai_next)
            {
                dest = socket(p_res->ai_family, p_res->ai_socktype, p_res->ai_protocol);
                if(dest == -1) continue;

                if(connect(dest, p_res->ai_addr, p_res->ai_addrlen) == 0) done = TRUE;
                close(dest);
            }

            freeaddrinfo(res);
        }
    }
    else
    {
        struct sockaddr_storage addr;

        if(atyp == ATYP_IPV4)
        {
            memcpy(&(((struct sockaddr_in*)&addr)->sin_addr.s_addr), host, 4);
            memcpy(&((struct sockaddr_in*)&addr)->sin_port, port, 2);

            ((struct sockaddr_in*)&addr)->sin_addr.s_addr = htonl((((struct sockaddr_in*)&addr)->sin_addr.s_addr));
        }
        else
        {
            memcpy(&(((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr), host, 4);
            memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, port, 2);

        }

        dest = socket((atyp == ATYP_IPV4) ? AF_INET : AF_INET6, SOCK_STREAM, 0);
        if(dest != -1)
        {
            if(connect(dest, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            {
                done = TRUE;
            }
        }
    }


    if(done)
    {
        struct sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        getsockname(dest, (struct sockaddr*)&addr, &addrLen);

        char repAtyp;
        char repHost[HOST_LEN + 1];
        char repPort[PORT_LEN + 1];

        if(addr.ss_family == AF_INET)
        {
            repAtyp = ATYP_IPV4;
            memcpy(repHost, (void*)&((struct sockaddr_in*)&addr)->sin_addr.s_addr, 4);
            memcpy(repPort, (void*)&((struct sockaddr_in*)&addr)->sin_port, 2);
        }
        else
        {
            repAtyp = ATYP_IPV6;
            memcpy(repHost, (void*)&((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr, 16);
            memcpy(repPort, (void*)&((struct sockaddr_in6*)&addr)->sin6_port, 4);
        }

        SOCKS_reply(client, REP_SUCCEEDED, repAtyp, repHost, repPort);
    }
    else
    {
        SOCKS_reply(client, REP_GENERAL_ERROR, atyp, host, port);
    }
}

bool SOCKS_bind(fd_t client, const char* host, const char* port)
{

}


bool SOCKS_udp_associate(fd_t client, const char* host, const char* port)
{
    /* PASS */
}
