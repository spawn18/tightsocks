#include "../include/convert.h"

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

