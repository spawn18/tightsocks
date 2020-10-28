#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

extern int MAX_CONNECTIONS;
extern method_t METHOD_PREFERED;
extern unsigned short PORT;
extern option_t OPTIONS;
extern decline_t DECLINES;

inline bool is_opt_set(option_t opt)
{
    return OPTIONS & opt;
}

inline void set_opt(option_t opt)
{
    OPTIONS |= opt;
}

inline void unset_opt(option_t opt)
{
    OPTIONS &= ~opt;
}


inline bool is_decline_set(decline_t dec)
{
    return DECLINES & dec;
}

inline void set_decline(decline_t dec)
{
    DECLINES |= dec;
}

inline void unset_decline(decline_t dec)
{
    DECLINES &= ~dec;
}

#endif 
