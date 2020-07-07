#ifndef SOCKS5_SERVER_CONVERT_H
#define SOCKS5_SERVER_CONVERT_H

#include <netinet/in.h>

void* getportss(struct sockaddr_storage* ss);

void* getaddrss(struct sockaddr_storage* ss);

#endif //SOCKS5_SERVER_CONVERT_H
