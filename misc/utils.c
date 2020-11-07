#include "defs.h"
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

void req_to_str(const request_t *req, char* host, char* port)
{
    if(req->ATYP == ATYP_IPV4)
    {
        strncpy(host, req->DSTADDR, 4);
    }
    else if(req->ATYP == ATYP_IPV4)
    {
        strncpy(host, req->DSTADDR, 16);
    }
    else
    {
        strncpy(host, &req->DSTADDR[1], req->DSTADDR[0]);
    }

    unsigned short p;
    p = req->DSTPORT[0] | (req->DSTPORT[1] << 8);
    p = ntohs(p);
    itos(p, port);
}

void addr_to_str(const struct sockaddr_storage *addr, char* host, char* port)
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

void cmd_to_str(cmd_t cmd, char *str)
{
    if(cmd == CMD_CONNECT)
    {
        strcpy(str, "CONNECT");
    }
    else if(cmd == CMD_BIND)
    {
        strcpy(str, "BIND");
    }
    else
    {
        strcpy(str, "\"UDP ASSOCIATE\"");
    }
}

void atyp_to_str(atyp_t atyp, char *str)
{
    if(atyp == ATYP_IPV4)
    {
        strcpy(str, "CONNECT");
    }
    else if(atyp == ATYP_IPV6)
    {
        strcpy(str, "BIND");
    }
    else
    {
        strcpy(str, "\"UDP ASSOCIATE\"");
    }
}