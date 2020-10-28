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


void SOCKS_connect(fd_t client, char* req)
{
    atyp_t atyp;
    char* host = NULL;
    char* port = NULL;

    parse_req(req, &atyp, &host, &port);

    fd_t dest;
    bool done = FALSE;
    rep_t reply = REP_GENERAL_FAILURE;

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

                if(dest == -1)
                {
                    reply = REP_GENERAL_FAILURE;
                    continue;
                }

                if(connect(dest, p_res->ai_addr, p_res->ai_addrlen) == 0)
                {
                    done = TRUE;
                    break;
                }
                else
                {
                    if      (errno == EHOSTUNREACH) reply = REP_HOST_UNREACHABLE;
                    else if (errno == ENETUNREACH)  reply = REP_NETWORK_UNREACHABLE;
                    else if (errno == ECONNREFUSED) reply = REP_CONNECTION_REFUSED;
                }

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
            addr.ss_family = AF_INET;
            memcpy(&(((struct sockaddr_in*)&addr)->sin_addr.s_addr), host, 4);
            memcpy(&((struct sockaddr_in*)&addr)->sin_port, port, 2);

            dest = socket(AF_INET, SOCK_STREAM, 0);
        }
        else
        {
            addr.ss_family = AF_INET6;
            memcpy(&(((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr), host, 4);
            memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, port, 2);

            dest = socket(AF_INET6, SOCK_STREAM, 0);
        }

        if(dest != -1)
        {
            if(connect(dest, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            {
                done = TRUE;
            }
            else
            {
                if(errno == EHOSTUNREACH) reply = REP_HOST_UNREACHABLE;
                else if(errno == ENETUNREACH) reply = REP_NETWORK_UNREACHABLE;
                else if(errno == ECONNREFUSED) reply = REP_CONNECTION_REFUSED;
            }
        }
    }


    if(done)
    {
        struct sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);

        getsockname(dest, (struct sockaddr*)&addr, &addrLen);

        atyp_t repAtyp;
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

        if(SOCKS_reply(client, REP_SUCCEEDED, repAtyp, repHost, repPort))
        {
            struct pollfd fds[2];

            fds[0].fd = client;
            fds[0].events = POLLIN | POLLOUT;

            fds[1].fd = dest;
            fds[1].events = POLLIN | POLLOUT;

            char buf[BUFSIZE];

            while(1)
            {
                int s = poll(fds, 2, 10000);
                if(s > 0)
                {
                    if(fds[0].revents & POLLIN)
                    {
                        if(fds[1].revents & POLLOUT)
                        {
                            int b = recv(fds[0].fd, buf, BUFSIZE, 0);
                            if(b > 0) send_all(fds[1].fd, buf, b);
                            else break;
                        }
                    }

                    if(fds[1].revents & POLLIN)
                    {
                        if(fds[0].revents & POLLOUT)
                        {
                            int b = recv(fds[1].fd, buf, BUFSIZE, 0);
                            if(b > 0) send_all(fds[0].fd, buf, b);
                            else break;
                        }
                    }
                }
                else break;
            }
        }

        close(dest);
    }
    else
    {
        SOCKS_reply(client, reply, atyp, host, port);
    }
}