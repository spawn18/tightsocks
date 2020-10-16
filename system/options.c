#include "protocol/method_exchange.h"
#include "options.h"

int METHOD_PREFERED = METHOD_NOAUTH;
int MAX_CONNECTIONS = 1024;
unsigned short PORT = 1080;

static unsigned int FLAGS = 0x0;

bool is_opt_set(option_t opt)
{
    return FLAGS & opt;
}

void set_opt(option_t opt)
{
    FLAGS |= opt;
}

void unset_opt(option_t opt)
{
    FLAGS &= ~opt;
}
