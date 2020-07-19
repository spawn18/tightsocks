#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "../include/convert.h"
#include "../include/types.h"

void* getaddrss(struct sockaddr_storage* ss)
{
    if(ss->ss_family == AF_INET)
    {
        return &(((struct sockaddr_in*)&ss)->sin_addr);
    }
    return &(((struct sockaddr_in6*)&ss)->sin6_addr);
}

void* getportss(struct sockaddr_storage* ss)
{
    if(ss->ss_family == AF_INET)
    {
        return &(((struct sockaddr_in*)&ss)->sin_port);
    }
    return &(((struct sockaddr_in6*)&ss)->sin6_port);
}

void fill_ss(struct sockaddr_storage* ss, ADDRESS_TYPE address_type, address_t address, port_t port)
{
    char* tmp;
    if(address_type == ADDRESS_TYPE_IPV4)
    {
        tmp = malloc(4 + 3);
        strcpy(tmp, &address[0]);
        for(int i = 1; i < 4; i++)
        {
            strcat(tmp, &address[i]);
            if(i != 3) strcat(tmp, (const char *) '.');
        }
    }
    else
    {
        tmp = malloc(16 + 15);
        strcpy(tmp, &address[0]);
        for(int i = 1; i < 16; i += 2)
        {
            strcat(tmp, &address[i]);
            if(i != 15) strcat(tmp, (const char *) ':');
        }
    }

    memset(ss, 0, sizeof(*ss));

    inet_pton((address_type == ADDRESS_TYPE_IPV4) ? AF_INET : AF_INET6, tmp, getaddrss(ss));
    memcpy(getportss(ss), (void*)port, 2);
}
