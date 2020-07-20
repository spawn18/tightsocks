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

void fill_ss(struct sockaddr_storage* ss, ADDRESS_TYPE address_type, size_t address_size, address_t address, port_t port)
{
    char* tmp;
    tmp = malloc(address_size + address_size-1);

    strcpy(tmp, &address[0]);
    for(int i = 1; i < address_size; i += (address_type == ADDRESS_TYPE_IPV4) ? 1 : 2)
    {
        strcat(tmp, &address[i]);
        if(i != address_size-1)
        {
            strcat(tmp, (address_type == ADDRESS_TYPE_IPV4) ? (const char *) '.' : (const char*)':');
        }
    }

    memset(ss, 0, sizeof(*ss));

    inet_pton((address_type == ADDRESS_TYPE_IPV4) ? AF_INET : AF_INET6, tmp, getaddrss(ss));
    memcpy(getportss(ss), (void*)port, 2);
}
