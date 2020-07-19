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
extern const int MAX_CLIENTS;
extern int FLAGS;


typedef enum OPTS : unsigned char
{
    OPT_IPV6 = (1 << 0),
    OPT_IPV4 = (1 << 1),
    OPT_VERBOSE = (1 << 2),
    OPT_DAEMON = (1 << 3),
    OPT_MAX_CLIENTS = (1 << 4),
} OPTS;

typedef enum MSGTYPE : unsigned char
{
    MSGTYPE_ERROR = 24,
    MSGTYPE_OK,
    MSGTYPE_WARNING,
    MSGTYPE_INFO
} MSGTYPE;

typedef enum METHODS : unsigned char
{
    METHODS_FIELD_VERSION = 0,
    METHODS_FIELD_NMETHODS,

    METHODS_VAR_NOAUTH = 0,
    METHODS_VAR_GSSAPI,
    METHODS_VAR_USERPASS,
    METHODS_VAR_NOMETHOD = 0xFF,

} METHODS;


typedef enum COMMAND_TYPE : unsigned char
{
    COMMAND_TYPE_CONNECT = 1,
    COMMAND_TYPE_BIND,
    COMMAND_TYPE_UDP_ASSOCIATE
} COMMAND_TYPE;

typedef enum ADDRESS_TYPE : unsigned char
{
    ADDRESS_TYPE_IPV4 = 1,
    ADDRESS_TYPE_DOMAINNAME = 3,
    ADDRESS_TYPE_IPV6,
} ADDRESS_TYPE;

typedef enum REPLY : unsigned char
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

typedef void (*request_p)(ADDRESS_TYPE, address_t, port_t );

typedef struct socks_request
{
    char version;
    char command;
    ADDRESS_TYPE address_type;
    size_t address_size;
    char* address;
    short port;
} socks_request_t;

#endif
