#include "system/args.h"
#include "system/server.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    handle_args(argc, argv);
    serve();

    return 0;
}