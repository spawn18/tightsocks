#ifndef HOMESOCKS_OPTIONS_H
#define HOMESOCKS_OPTIONS_H

#include "misc/defs.h"

typedef enum OPTION_T
{
    OPT_IP4             = (1 << 1),
    OPT_IP6             = (1 << 2),
    OPT_PORT            = (1 << 3),
    OPT_MAX_CONNS       = (1 << 4),
    OPT_LOG             = (1 << 5),
    OPT_VERBOSE         = (1 << 6),
    OPT_BUFSIZE			= (1 << 7),
    OPT_AUTH			= (1 << 8)
} option_t;

extern int BUFSIZE;
extern int MAX_CONNS;
extern unsigned short PORT;
extern char* USERNAME;
extern char* PASSWORD;

extern option_t OPTIONS;

#define IS_OPT_SET(opt) (OPTIONS & opt)
#define SET_OPT(opt) (OPTIONS |= opt)
#define UNSET_OPT(opt) (OPTIONS &= ~opt)


#endif 
