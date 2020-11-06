#include "defs.h"
#include "utils.h"
#include "protocol/request.h"

#include <arpa/inet.h>

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


void unload_addr(const struct sockaddr_storage *addr, char* host, char* port)
{
    if(addr->ss_family == AF_INET)
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in*)&addr)->sin_addr.s_addr, host, INET6_ADDRSTRLEN);
    }
    else
    {
        inet_ntop(addr->ss_family, (void*)&((struct sockaddr_in6*)&addr)->sin6_addr.s6_addr, host, INET_ADDRSTRLEN);
    }

    unsigned short sPort;

    if(addr->ss_family == AF_INET)
    {
        sPort = ((struct sockaddr_in*)&addr)->sin_port | (((struct sockaddr_in*)&addr)->sin_port << 8);
    }
    else
    {
        sPort = ((struct sockaddr_in6*)&addr)->sin6_port | (((struct sockaddr_in6*)&addr)->sin6_port << 8);
    }

    sPort = ntohs(sPort);
    itos(sPort, port);
}

void load_addr()
{

}