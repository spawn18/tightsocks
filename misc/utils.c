#include "defs.h"
#include "utils.h"
#include "protocol/request.h"

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <system/log.h>
#include <protocol/reply.h>
#include <stdio.h>

static void fmt_ipv4(const char* hostIn, char* hostOut)
{
    *hostOut = '\0';

    for(const char* octet = hostIn; octet < octet+4; octet++)
    {
        char octetStr[3 + 1];
        itos(*octet, octetStr);
        strcat(hostOut, octetStr);
        if(octet != hostIn+3) strcat(hostOut, ".");
    }
}


static void dec_to_hex(int d, char* hex)
{
    int i = 0;

    while(d != 0)
    {
        int last = 0;
        last = d % 16;

        if(last < 10) hex[i] = last + 48;
        else hex[i] = last + 55;

        d /= 16;
        i++;
    }

    hex[i] = '\0';


}

static void fmt_ipv6(const char* hostIn, char* hostOut)
{
    *hostOut = '\0';

    for(const char* group = hostIn; group < group+16; group += 2)
    {
        char s1[3];
        char s2[3];

        dec_to_hex(group[0], s1);
        dec_to_hex(group[1], s2);

        strcat(hostOut, s1);
        strcat(hostOut, s2);

        if(group < 14)
            strcat(hostOut, ":");
    }

}

static int count_digits(int num)
{
    int count = 1;
    while(num != 0)
    {
        num /= 10;
        count++;
    }
    return count;
}

void itos(int d, char* s)
{
    int n = count_digits(d);
    s[n] = '\0';

    for(int i = n-1; i >= 0; i--)
    {
        char mod = (char)(d % 10);
        s[i] = mod;

        d /= 10;
    }
}


void fmt_req(const char addrType, const char* hostIn, const char* portIn, char* hostOut, char* portOut)
{
    /* Host */
    if(addrType == ADDR_TYPE_DOMAINNAME)
    {
        strncpy(hostOut, &hostIn[1], hostIn[0]);
    }
    else if(addrType == ADDR_TYPE_IPV4)
    {
        fmt_ipv4(hostIn, hostOut);
    }
    else
    {
        fmt_ipv6(hostIn, hostOut);
    }


    /* Port */
    short port = portIn[0] | (portIn[1] << 8);
    port = ntohs(port);
    itos(port, portOut);
}





/* TODO: "192.168" and so on is each digits a char */
void ssfreply(const struct sockaddr_storage* ss, addr_type_t addrType, char* host, char* port)
{
    char hostP[MAX_HOST_SIZE + 1];
    char portP[MAX_PORT_SIZE + 1];

    inet_ntop(ss->ss_family, (ss->ss_family == AF_INET) ? &(((struct sockaddr_in*)&ss)->sin_addr) :
                &(((struct sockaddr_in6*)&ss)->sin6_addr), hostP, MAX_HOST_SIZE);

    char delim = (ss->ss_family == AF_INET) ? '.' : ':';
    char addrLen = (ss->ss_family == AF_INET) ? P_IPV4_ADDR_LEN : P_IPV6_ADDR_LEN;
    remove_char_N(hostP, host, '.');


}

void remove_char(const char* strIn, char* strOut, char ch)
{
    do
    {
        if(*strIn == ch) ++strIn;
        *(strOut++) = *(strIn++);
    }
    while(*strIn);
}

void remove_char_n(const char* strIn, size_t len, char* strOut, char ch)
{
    for(int i = 0; i < len; i++)
    {
        if(*strIn == ch) ++strOut;
        *(strOut++) = *(strIn++);
    }
}

