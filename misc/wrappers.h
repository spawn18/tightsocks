
#ifndef ACE_WRAPPERS_H
#define ACE_WRAPPERS_H

#include "defs.h"
#include "netdb.h"

bool find_sockets(const char* host,
                  const char* port,
                  int family,
                  int socketType,
                  int protocol,
                  int flags,
                  struct addrinfo *gaiRes);

bool connect_socket(fd_t *sock, struct addrinfo *res);
bool bind_socket(fd_t *sock, struct addrinfo *res);
void get_peer_info(fd_t socket, char* addrType, char* host, char* port);

#endif 
