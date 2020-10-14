#include <stdlib.h>
#include "system/args.h"
#include "system/server.h"

#include "system/log.h"

void clean_up()
{
    if(is_log_open())
    {
        log_save();
        log_close();
    }
}

int main(int argc, char** argv)
{
    if(atexit(clean_up) == 0)
    {
        handle_args(argc, argv);
        serve();
    }

    return 0;
}
