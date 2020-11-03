#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

#include "misc/defs.h"

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


extern int MAX_CONNECTIONS;
extern unsigned short PORT;
extern option_t OPTIONS;


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


#endif 
