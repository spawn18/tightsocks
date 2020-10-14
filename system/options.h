
#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H


typedef enum OPTIONS
{
    OPT_ENUM_BEGIN = 0,

    OPT_IPV6,
    OPT_IPV4,
    OPT_LOG,
    OPT_DAEMON,
    OPT_MAX_CONNECTIONS,
    OPT_METHOD,

    OPT_ENUM_END
} option_t;

extern int MAX_CONNECTIONS;
extern int METHOD_PREFERED;

bool is_opt_set(option_t);
void set_opt(option_t);
void unset_opt(option_t);


#endif 
