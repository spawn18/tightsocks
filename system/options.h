#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

extern int MAX_CONNECTIONS;
extern method_t METHOD_PREFERED;
extern int BUFSIZE;
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
