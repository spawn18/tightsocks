#include "utils.h"
#include "protocol/request.h"

#include <arpa/inet.h>
#include <string.h>

static int count_digits(int num)
{
    int count = 0;
    while(num != 0)
    {
        num /= 10;
        count++;
    }
    return count;
}

static void itos(int d, char* s)
{
    int n = count_digits(d);
    s[n] = '\0';

    for(int i = n-1; i >= 0; i--)
    {
        s[i] =  (char)(d % 10 + 48);
        d /= 10;
    }
}

void req_addr_to_str(atyp_t reqAtyp, const char* reqHost, const char* reqPort, char* host, char* port)
{
    struct sockaddr_storage addr;

    if(reqAtyp == ATYP_IPV4)
    {
        addr.ss_family = AF_INET;
        memcpy(&((struct sockaddr_in*)&addr)->sin_addr, reqHost, 4);
        memcpy(&((struct sockaddr_in*)&addr)->sin_port, reqPort, 2);
        addr_to_str(&addr, host, port);
    }
    else if(reqAtyp == ATYP_IPV6)
    {
        addr.ss_family = AF_INET6;
        memcpy(&((struct sockaddr_in6*)&addr)->sin6_addr, reqHost, 16);
        memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, reqPort, 2);
        addr_to_str(&addr, host, port);
    }
    else
    {
        memcpy(host, &reqHost[1], reqHost[0]);

        unsigned short p;
        p = reqPort[0] | (reqPort[1] << 8);
        p = ntohs(p);
        itos(p, port);
    }
}

void addr_to_str(const struct sockaddr_storage *addr, char* host, char* port)
{
    if(addr->ss_family == AF_INET)
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in*)&addr)->sin_addr, host, INET_ADDRSTRLEN);
    }
    else
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in6*)&addr)->sin6_addr, host, INET6_ADDRSTRLEN);
    }

    unsigned short p;

    if(addr->ss_family == AF_INET)
    {
        p = ((struct sockaddr_in*)&addr)->sin_port | (((struct sockaddr_in*)&addr)->sin_port << 8);
    }
    else
    {
        p = ((struct sockaddr_in6*)&addr)->sin6_port | (((struct sockaddr_in6*)&addr)->sin6_port << 8);
    }

    p = ntohs(p);
    itos(p, port);
}

