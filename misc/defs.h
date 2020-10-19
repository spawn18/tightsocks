#ifndef ACE_DEFS_H
#define ACE_DEFS_H

typedef int fd_t;
typedef int bool;

#define TRUE 1
#define FALSE 0

#define IPV4_LEN 4
#define IPV6_LEN 16

#define SOCKS_VER 5

#define HOST_LEN 255
#define PORT_LEN 5

#define REQ_LEN 261

typedef enum CMD
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,

} cmd_t;

typedef enum METHODS
{
    METHOD_NOAUTH = 0,
    METHOD_GSSAPI,
    METHOD_USERPASS ,
    METHOD_NOMETHOD = 0xFF,
} met_t;


typedef enum REP
{
    REP_SUCCEEDED = 0,
    REP_GENERAL_ERROR,
} rep_t;

typedef enum ATYP
{
    ATYP_IPV4 = 1,
    ATYP_DOMAINNAME = 3,
    ATYP_IPV6,
} atyp_t;

#endif 
