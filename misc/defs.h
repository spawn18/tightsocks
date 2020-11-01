#ifndef ACE_DEFS_H
#define ACE_DEFS_H

typedef int fd_t;
typedef int bool;

#define TRUE 1
#define FALSE 0
#define SOCKS_VER 5
#define HOST_LEN 255
#define PORT_LEN 5
#define REQUEST_LEN 261
#define AUTH_LEN 513
#define REPLY_LEN 261
#define METHODS_LEN 257
#define LOG_ENTRY_LEN 1024
#define CMD_LEN 13
#define ATYP_LEN 10
#define DATE_LEN 255
#define TIME_LEN 255

typedef enum CMD_T
{
    CMD_CONNECT = 1,
    CMD_BIND,
    CMD_UDP_ASSOCIATE,
} cmd_t;

typedef enum METHOD_T
{
    METHOD_NOAUTH = 0,
    METHOD_GSSAPI,
    METHOD_USERPASS ,
    METHOD_NOMETHOD = 0xFF,
} method_t;

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

typedef enum OPTION_T
{
    OPT_IP6,
    OPT_IP4,
    OPT_PORT,
    OPT_MAX_CONNECTIONS,
    OPT_METHOD,
    OPT_LOG,
    OPT_DECLINE,
    OPT_BUFSIZE
} option_t;

typedef enum DECLINE_T
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

typedef struct REPLY_T
{
    char    VER;
    rep_t   REP;
    char    RSV;
    atyp_t  ATYP;
    char    BNDADDR[HOST_LEN + 1];
    char    BNDPORT[PORT_LEN + 1];
} reply_t;

typedef struct REQUEST_T
{
    char    VER;
    cmd_t   CMD;
    char    RSV;
    atyp_t  ATYP;
    char    DSTADDR[HOST_LEN + 1];
    char    DSTPORT[PORT_LEN + 1];
} request_t;



#endif 
