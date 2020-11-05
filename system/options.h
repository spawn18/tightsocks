#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

#include "misc/defs.h"

typedef enum OPTION_T
{
    OPT_NOOPTS          = 0,
    OPT_IP4             = (1 << 0),
    OPT_IP6             = (1 << 1),
    OPT_PORT            = (1 << 2),
    OPT_MAX_CONNECTIONS = (1 << 3),
    OPT_USERPASS        = (1 << 4),
    OPT_FIREWALL        = (1 << 5),
    OPT_LOG             = (1 << 6)
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
