
#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H


typedef enum OPTIONS
{
    OPT_IP6,
    OPT_IP4,
    OPT_PORT,
    OPT_MAX_CONNECTIONS,
    OPT_METHOD,
} option_t;

extern int MAX_CONNECTIONS;
extern int METHOD_PREFERED;
extern unsigned short PORT;

bool is_opt_set(option_t);
void set_opt(option_t);
void unset_opt(option_t);


#endif 
