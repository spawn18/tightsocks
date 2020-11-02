#include "firewall.h"

#include "misc/defs.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static FILE* fwFile = NULL;
static pthread_mutex_t mut;

bool fw_open()
{
    fwFile = fopen("blacklist.txt", "r");
    return (fwFile != NULL) ? TRUE : FALSE;
}

bool fw_find(char* entry)
{

}
