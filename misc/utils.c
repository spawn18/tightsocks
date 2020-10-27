#include "defs.h"
#include "utils.h"
#include "protocol/request.h"

#include <arpa/inet.h>
#include <stdio.h>


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


static void itos(int d, char* s)
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


static void remove_char(const char* strIn, char* strOut, char ch)
{
    do
    {
        if(*strIn == ch) ++strIn;
        *(strOut++) = *(strIn++);
    }
    while(*strIn);
}


void fmt_addr(struct sockaddr_storage addr, char* host, char* port)
{
    inet_ntop(addr.ss_family, (void*)&addr, host, HOST_LEN+1);

    short sPort;

    if(addr.ss_family == AF_INET)
    {
        sPort = ntohs(((struct sockaddr_in*)&addr)->sin_port |
                      (((struct sockaddr_in*)&addr)->sin_port << 8));
    }
    else
    {
        sPort = ntohs(((struct sockaddr_in6*)&addr)->sin6_port |
                      (((struct sockaddr_in6*)&addr)->sin6_port << 8));
    }

    itos(sPort, port);
}
