#ifndef ACE_OPTIONS_H
#define ACE_OPTIONS_H

extern int MAX_CONNECTIONS;
extern int METHOD_PREFERED;
extern unsigned short PORT;

bool is_opt_set(option_t);
void set_opt(option_t);
void unset_opt(option_t);

#endif 
