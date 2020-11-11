#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

#include "misc/defs.h"

typedef enum OPTION_T
{
    OPT_IP4             = (1 << 1),
    OPT_IP6             = (1 << 2),
    OPT_PORT            = (1 << 3),
    OPT_MAX_CONNS       = (1 << 4),
    OPT_USERPASS        = (1 << 5),
    OPT_FIREWALL        = (1 << 6),
    OPT_LOG             = (1 << 7)
} option_t;

extern int MAX_CONNS;
extern unsigned short PORT;
extern option_t OPTIONS;
extern char USERNAME[255 + 1];
extern char PASSWORD[255 + 1];

#define IS_OPT_SET(opt) (OPTIONS & opt)
#define SET_OPT(opt) (OPTIONS |= opt)
#define UNSET_OPT(opt) OPTIONS &= ~opt


#endif 
