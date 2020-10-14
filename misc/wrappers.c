#include "wrappers.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>


bool get_connectee_info(fd_t sock, char* host, char* port)
{
    struct sockaddr_storage bndAddr;
    memset(&bndAddr, 0, sizeof(struct sockaddr_storage));

    socklen_t bndAddrSize = sizeof(bndAddr);
    if(getsockname(sock, (struct sockaddr*)&bndAddr, &bndAddrSize) != 0)
    {
        return FALSE;
    }

    return TRUE;
}


bool find_sockets(const char* host,
                  const char* port,
                  int family,
                  int socketType,
                  int protocol,
                  int flags,
                  struct addrinfo* gaiRes)
{
    struct addrinfo gaiInfo;
    memset(&gaiInfo, 0, sizeof(struct addrinfo));

    gaiInfo.ai_family    = family;
    gaiInfo.ai_socktype  = socketType;
    gaiInfo.ai_protocol  = protocol;
    gaiInfo.ai_canonname = NULL;
    gaiInfo.ai_flags     = flags;

    if(getaddrinfo(host, port, &gaiInfo, &gaiRes) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




bool connect_socket(fd_t *s, struct addrinfo* res)
{
    struct addrinfo* p_gaiRes = NULL;
    for(p_gaiRes = res; p_gaiRes != NULL; p_gaiRes = p_gaiRes->ai_next)
    {
        *s = socket(p_gaiRes->ai_family, p_gaiRes->ai_socktype, p_gaiRes->ai_protocol);
        if(*s == -1) continue;

        if(connect(*s, p_gaiRes->ai_addr, p_gaiRes->ai_addrlen) == 0)
        {
            break;
        }
        close(*s);
    }

    freeaddrinfo(res);

    if(p_gaiRes == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

bool bind_socket(fd_t *sock, struct addrinfo *res)
{
    struct addrinfo* p_gaiRes = NULL;
    for(p_gaiRes = res; p_gaiRes != NULL; p_gaiRes = p_gaiRes->ai_next)
    {
        *sock = socket(p_gaiRes->ai_family, p_gaiRes->ai_socktype, p_gaiRes->ai_protocol);
        if(*sock == -1) continue;

        if(bind(*sock, p_gaiRes->ai_addr, p_gaiRes->ai_addrlen) == 0) break;
        close(*sock);
    }

    freeaddrinfo(res);

    if(p_gaiRes == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

void get_peer_info(fd_t socket, char* addrType, char* host, char* port)
{
    struct sockaddr_storage addr;
    socklen_t addrLen = sizeof(addr);

    if(getpeername(socket, (struct sockaddr*)&addr, &addrLen) == 0)
    {
        if(addr.ss_family == AF_INET)
        {
            inet_ntop(AF_INET, (void*)&(((struct sockaddr_in*)&addr)->sin_addr), host, MAX_HOST_LEN + 1);
        }
        else
        {
            inet_ntop(AF_INET6, (void*)&(((struct sockaddr_in6*)&addr)->sin6_addr), host, MAX_HOST_LEN + 1);
        }

        int portD = ntohs((((struct sockaddr_in*)&addr)->sin_port));
        itos(portD, port);
    }
}