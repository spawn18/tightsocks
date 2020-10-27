#include "protocol/method_exchange.h"
#include "options.h"

int METHOD_PREFERED = METHOD_NOAUTH;
int MAX_CONNECTIONS = 1024;
unsigned short PORT = 1080;


static option_t OPTIONS = 0;
static decline_t DECLINES = 0;

bool is_opt_set(option_t opt)
{
    return OPTIONS & opt;
}

void set_opt(option_t opt)
{
    OPTIONS |= opt;
}

void unset_opt(option_t opt)
{
    OPTIONS &= ~opt;
}


bool is_decline_set(decline_t dec)
{
    return DECLINES & dec;
}

void set_decline(decline_t dec)
{
    DECLINES |= dec;
}

void unset_decline(decline_t dec)
{
    DECLINES &= ~dec;
}