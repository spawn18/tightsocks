#ifndef SOCKS5_SERVER_TYPES_H
#define SOCKS5_SERVER_TYPES_H

typedef int fd_t;
typedef int res_t;
typedef char index_t;

enum OPTS : unsigned char
{
    BM_OPT_IPV6 = 0x01,
    BM_OPT_IPV4 = 0x02,
    BM_OPT_VERBOSE = 0x04,
    BM_OPT_DAEMON = 0x08,
    BM_OPT_MAX_CLIENTS = 0x10
};

enum MSGTYPE : unsigned char
{
    MSGTYPE_ERROR = 24,
    MSGTYPE_OK,
    MSGTYPE_WARNING,
    MSGTYPE_INFO
};

enum METHODS : unsigned char
{
    METHODS_FIELD_VERSION = 0,
    METHODS_FIELD_NMETHODS,

    METHODS_VAR_NOAUTH = 0,
    METHODS_VAR_GSSAPI,
    METHODS_VAR_USERPASS,
    METHODS_VAR_NOMETHOD = 0xFF,

};


enum COMMANDS : unsigned char
{
    CONNECT = 0x01,
    BIND,
    UDP_ASSOCIATE
};

enum ADDRTYPE : unsigned char
{
    IPV4 = 0x01,
    DOMAINNAME = 0x03,
    IPV6,
};

#endif
