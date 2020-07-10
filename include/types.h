#ifndef TYPES_H
#define TYPES_H

typedef int fd_t;
typedef int res_t;
typedef char index_t;
typedef char method_t;

const char SOCKS_VERSION = 0x05;
const char* SOCKS_PORT = "1080";
const int BUFSIZE = 512;
const int MAX_CLIENTS = 10;

enum OPTS : unsigned char
{
    OPT_IPV6 = 0x01,
    OPT_IPV4 = 0x02,
    OPT_VERBOSE = 0x04,
    OPT_DAEMON = 0x08,
    OPT_MAX_CLIENTS = 0x10
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


enum REQTYPE : unsigned char
{
    REQTYPE_CONNECT = 0x01,
    REQTYPE_BIND,
    REQTYPE_UDP_ASSOCIATE
};

enum ADDRTYPE : unsigned char
{
    ADDRTYPE_IPV4 = 0x01,
    ADDRTYPE_DOMAINNAME = 0x03,
    ADDRTYPE_IPV6,
};

#endif
