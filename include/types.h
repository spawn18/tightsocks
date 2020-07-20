#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef int fd_t;
typedef int res_t;
typedef unsigned int index_t;
typedef char method_t;

typedef const char* address_t;
typedef short port_t;

extern const char SOCKS_VERSION;
extern const char* SOCKS_PORT;
extern const int BUFSIZE;
extern unsigned int MAX_CLIENTS;
extern unsigned int FLAGS;


typedef enum OPTS
{
    OPT_IPV6 = (1 << 0),
    OPT_IPV4 = (1 << 1),
    OPT_VERBOSE = (1 << 2),
    OPT_DAEMON = (1 << 3),
    OPT_MAX_CLIENTS = (1 << 4)
} OPTS;

typedef enum MSGTYPE
{
    MSGTYPE_ERROR = 24,
    MSGTYPE_OK,
    MSGTYPE_WARNING,
    MSGTYPE_INFO
} MSGTYPE;

typedef enum METHODS
{
    METHODS_NOAUTH = 0,
    METHODS_GSSAPI,
    METHODS_USERPASS,
    METHODS_NOMETHOD = 0xFF,
} METHODS;

typedef enum COMMAND
{
    COMMAND_CONNECT = 1,
    COMMAND_BIND,
    COMMAND_UDP_ASSOCIATE
} COMMAND;

typedef enum ADDRESS_TYPE
{
    ADDRESS_TYPE_IPV4 = 1,
    ADDRESS_TYPE_DOMAINNAME = 3,
    ADDRESS_TYPE_IPV6,
} ADDRESS_TYPE;

typedef enum REPLY
{
    REPLY_SUCCEEDED = 0,
    REPLY_GENERAL_ERROR,
    REPLY_CONNECTION_NOT_ALLOWED,
    REPLY_NETWORK_UNREACHABLE,
    REPLY_HOST_UNREACHEABLE,
    REPLY_CONNECTION_REFUSED,
    REPLY_TTL_EXPIRED,
    REPLY_COMMAND_NOT_SUPPORTED,
    REPLY_ADDRESS_TYPE_NOT_SUPPORTED,
} REPLY;

typedef char* socks_request_t;

typedef void (*socks_request_f)(socks_request_t);
#endif
