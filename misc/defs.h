#ifndef HOMESOCKS_DEFS_H
#define HOMESOCKS_DEFS_H

typedef int sock_t;
typedef int bool;

#define TRUE 1
#define FALSE 0
#define SOCKS_VER 5

#define INFO(m)   if(IS_OPT_SET(OPT_VERBOSE)) printf("[ INFO ] %s\n", m);
#define ERR(m, e) if(IS_OPT_SET(OPT_VERBOSE)) fprintf(stderr, "[ ERROR ] %s\n%s\n", m, e)
#define WARN(m)   if(IS_OPT_SET(OPT_VERBOSE)) printf("[ WARNING ]: %s\n", m);

#endif 
