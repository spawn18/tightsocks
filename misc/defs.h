#ifndef ACE_DEFS_H
#define ACE_DEFS_H

typedef int fd_t;
typedef int bool;

#define TRUE 1
#define FALSE 0
#define SOCKS_VER 5
#define HOST_LEN 255
#define PORT_LEN 5
#define REQ_LEN 261
#define AUTH_LEN 513

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
    REP_GENERAL_FAILURE,
    REP_NOT_ALLOWED_BY_RULESET,
    REP_NETWORK_UNREACHABLE,
    REP_HOST_UNREACHABLE,
    REP_CONNECTION_REFUSED,
    REP_TTL_EXPIRED,
    REP_COMMAND_NOT_SUPPORTED,
    REP_ADDRESS_TYPE_NOT_SUPPORTED,
} rep_t;

typedef enum ATYP
{
    ATYP_IPV4 = 1,
    ATYP_DOMAINNAME = 3,
    ATYP_IPV6,
} atyp_t;

#endif 
