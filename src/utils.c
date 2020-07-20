#include "../include/types.h"

unsigned int is_option_set(OPTS opts)
{
    return FLAGS & opts;
}

void set_option(OPTS opts)
{
    FLAGS |= opts;
}
