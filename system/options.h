#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

#include "misc/defs.h"

typedef enum OPTION_T
{
    OPT_IP4             = (1 << 1),
    OPT_IP6             = (1 << 2),
    OPT_PORT            = (1 << 3),
    OPT_MAX_CONNS = (1 << 4),
    OPT_USERPASS        = (1 << 5),
    OPT_LOG             = (1 << 6)
} option_t;


extern int MAX_CONNS;
extern unsigned short PORT;
extern option_t OPTIONS;
extern char USERNAME[255 + 1];
extern char PASSWORD[255 + 1];

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
