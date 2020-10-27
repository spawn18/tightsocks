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
#define REP_LEN 261
#define METHODS_LEN 257
#define LOG_ENTRY_LEN 1024
#define CMD_LEN 13
#define ATYP_LEN 10
#define DATE_LEN 255
#define TIME_LEN 255

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

typedef enum OPTIONS
{
    OPT_IP6,
    OPT_IP4,
    OPT_PORT,
    OPT_MAX_CONNECTIONS,
    OPT_METHOD,
    OPT_LOG,
    OPT_DECLINE,
} option_t;

typedef enum DECLINE_FIELDS
{
    DECLINE_CONNECT = 1,
    DECLINE_BIND,
    DECLINE_UDP_ASSOCIATE,
    DECLINE_IPV4,
    DECLINE_IPV6,
    DECLINE_DOMAINNAME
} decline_t;

typedef struct LOG_ENTRY_T
{
    char srcHost[HOST_LEN + 1];
    char srcPort[PORT_LEN + 1];
    char command[CMD_LEN + 1];
    char addrType[ATYP_LEN + 1];
    char dstHost[HOST_LEN + 1];
    char dstPort[PORT_LEN + 1];
} log_entry_t;

#endif 
