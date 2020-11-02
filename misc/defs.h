#ifndef ACE_DEFS_H
#define ACE_DEFS_H

typedef int socket_t;
typedef int bool;

#define TRUE 1
#define FALSE 0

#define SOCKS_VER 5
#define HOST_LEN 255
#define PORT_LEN 5
#define CMD_LEN 13
#define ATYP_LEN 10



typedef enum CMD_T
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,
} cmd_t;


typedef enum REP_T
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

typedef enum ATYP_T
{
    ATYP_IPV4 = 1,
    ATYP_DOMAINNAME = 3,
    ATYP_IPV6,
} atyp_t;





#endif 
