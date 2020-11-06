#include "connect.h"

#include "protocol/reply.h"
#include "network/io.h"
#include "system/options.h"

#include <string.h>
#include <netdb.h>
#include <unistd.h>


void SOCKS_connect(sock_t client, const request_t *req)
{
    sock_t host;

    reply_t reply = {0};
    reply.VER = SOCKS_VER;
    reply.REP = REP_GENERAL_FAILURE;
    reply.RSV = 0;
    reply.ATYP = 0;
    memset(reply.BNDADDR, 0, 255);
    memset(reply.BNDPORT, 0, 2);


    if(req->ATYP == ATYP_DOMAINNAME)
    {
        struct addrinfo gaiInfo;
        struct addrinfo* res = NULL;

        memset(&gaiInfo, 0, sizeof(struct addrinfo));

        gaiInfo.ai_family    = AF_INET | AF_INET6;
        gaiInfo.ai_socktype  = SOCK_STREAM;
        gaiInfo.ai_protocol  = 0;
        gaiInfo.ai_flags     = 0;

        if(getaddrinfo(&req->DSTADDR[1], req->DSTPORT, &gaiInfo, &res) == 0)
        {
            struct addrinfo* p_res = NULL;
            for(p_res = res; p_res != NULL; p_res = p_res->ai_next)
            {
                host = socket(p_res->ai_family, p_res->ai_socktype, p_res->ai_protocol);

                if(host == -1)
                {
                    continue;
                }

                if(connect(host, p_res->ai_addr, p_res->ai_addrlen) == 0)
                {
                    reply.REP = REP_SUCCEEDED;
                    break;
                }

                close(host);
            }

            freeaddrinfo(res);
        }
    }
    else
    {
        struct sockaddr_storage addr;

        if(req->ATYP == ATYP_IPV4)
        {
            addr.ss_family = AF_INET;
            memcpy(&(((struct sockaddr_in*)&addr)->sin_addr.s_addr), req->DSTADDR, 4);
            memcpy(&((struct sockaddr_in*)&addr)->sin_port, req->DSTPORT, 2);

            host = socket(AF_INET, SOCK_STREAM, 0);
        }
        else
        {
            addr.ss_family = AF_INET6;
            memcpy(&(((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr), req->DSTADDR, 4);
            memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, req->DSTPORT, 2);

            host = socket(AF_INET6, SOCK_STREAM, 0);
        }

        if(host != -1)
        {
            if(connect(host, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            {
                reply.REP = REP_SUCCEEDED;
            }
        }
    }


    if(reply.REP == REP_SUCCEEDED)
    {
        struct sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        getsockname(host, (struct sockaddr*)&addr, &addrLen);

        if(addr.ss_family == AF_INET)
        {
            reply.ATYP = ATYP_IPV4;
            memcpy(reply.BNDADDR, (void*)&((struct sockaddr_in*)&addr)->sin_addr.s_addr, 4);
            memcpy(reply.BNDPORT, (void*)&((struct sockaddr_in*)&addr)->sin_port, 2);
        }
        else
        {
            reply.ATYP = ATYP_IPV6;
            memcpy(reply.BNDADDR, (void*)&((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr, 16);
            memcpy(reply.BNDPORT, (void*)&((struct sockaddr_in6*)&addr)->sin6_port, 2);
        }
    }

    SOCKS_reply(client, &reply);

    if(reply.REP == REP_SUCCEEDED)
    {
        handle_io(client, host);
        close(host);
    }
}