#include "connect.h"

#include "protocol/reply.h"
#include "network/io.h"
#include "misc/utils.h"
#include "system/options.h"

#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>


void SOCKS_connect(socket_t client, const request_t *req)
{
    socket_t host;
    bool done = FALSE;

    reply_t reply;
    reply.REP = REP_GENERAL_FAILURE;

    if(req->ATYP == ATYP_DOMAINNAME)
    {
        struct addrinfo gaiInfo;
        struct addrinfo* res;

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
                    reply.REP = REP_GENERAL_FAILURE;
                    continue;
                }

                if(connect(host, p_res->ai_addr, p_res->ai_addrlen) == 0)
                {
                    done = TRUE;
                    break;
                }
                else
                {
                    if      (errno == EHOSTUNREACH) reply.REP= REP_HOST_UNREACHABLE;
                    else if (errno == ENETUNREACH)  reply.REP = REP_NETWORK_UNREACHABLE;
                    else if (errno == ECONNREFUSED) reply.REP = REP_CONNECTION_REFUSED;
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
                done = TRUE;
            }
            else
            {
                if(errno == EHOSTUNREACH) reply.REP = REP_HOST_UNREACHABLE;
                else if(errno == ENETUNREACH) reply.REP = REP_NETWORK_UNREACHABLE;
                else if(errno == ECONNREFUSED) reply.REP = REP_CONNECTION_REFUSED;
            }
        }
    }


    if(done)
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
            memcpy(reply.BNDPORT, (void*)&((struct sockaddr_in6*)&addr)->sin6_port, 4);
        }

        if(SOCKS_reply(client, &reply))
        {
            handle_io(client, host);
        }

        close(host);
    }
    else
    {
        SOCKS_reply(client, &reply);
    }
}