#include "utils.h"
#include "protocol/request.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

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

static void i_to_s(int d, char* s)
{
    int n = count_digits(d);
    s[n] = '\0';

    for(int i = n-1; i >= 0; i--)
    {
        s[i] =  (char)(d % 10 + 48);
        d /= 10;
    }
}

static int mem_to_i(void* mem, int n)
{
    char* cmem = (char*)mem;

    int r = 0;

    for(int i = 0; i < n; i++)
    {
        r |= cmem[i] << (8*i);
    }

    return r;
}

void req_host_to_p(atyp_t reqAtyp, const char* reqHost, const char* reqPort, char* host, char* port)
{
    struct sockaddr_storage addr = {0};

    if(reqAtyp == ATYP_IPV4)
    {
        addr.ss_family = AF_INET;
        memcpy(&((struct sockaddr_in*)&addr)->sin_addr.s_addr, reqHost, 4);
        memcpy(&((struct sockaddr_in*)&addr)->sin_port, reqPort, 2);
        host_to_p(&addr, host, port);
    }
    else if(reqAtyp == ATYP_IPV6)
    {
        addr.ss_family = AF_INET6;
        memcpy(&((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr, reqHost, 16);
        memcpy(&((struct sockaddr_in6*)&addr)->sin6_port, reqPort, 2);
        host_to_p(&addr, host, port);
    }
    else
    {
        memcpy(host, &reqHost[1], reqHost[0]);
        unsigned short p = mem_to_i((void*)reqPort, 2);
        p = ntohs(p);
        i_to_s(p, port);
    }
}

void host_to_p(const struct sockaddr_storage *addr, char* host, char* port)
{
    unsigned short p;

    if(addr->ss_family == AF_INET)
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in*)addr)->sin_addr, host, INET_ADDRSTRLEN);
        p = ((struct sockaddr_in*)addr)->sin_port;
    }
    else
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in6*)addr)->sin6_addr, host, INET6_ADDRSTRLEN);
        p = ((struct sockaddr_in6*)addr)->sin6_port;
    }

    p = ntohs(p);
    i_to_s(p, port);

}
