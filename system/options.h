#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

typedef enum OPTION_T
{
    OPT_IP6,
    OPT_IP4,
    OPT_PORT,
    OPT_MAX_CONNECTIONS,
    OPT_USERPASS,
    OPT_FIREWALL,
    OPT_LOG,
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

extern int MAX_CONNECTIONS;
extern method_t METHOD_PREFERED;
extern unsigned short PORT;

extern option_t OPTIONS;
extern decline_t DECLINES;

static inline bool is_opt_set(option_t opt)
{
    return OPTIONS & opt;
}

static inline void set_opt(option_t opt)
{
    OPTIONS |= opt;
}

static inline void unset_opt(option_t opt)
{
    OPTIONS &= ~opt;
}

static inline bool is_decline_set(decline_t dec)
{
    return DECLINES & dec;
}

static inline void set_decline(decline_t dec)
{
    DECLINES |= dec;
}

static inline void unset_decline(decline_t dec)
{
    DECLINES &= ~dec;
}

#endif 
