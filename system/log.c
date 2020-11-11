#include "log.h"
#include "misc/defs.h"
#include "misc/chrono.h"

#include <stdio.h>
#include <memory.h>
#include <pthread.h>

static FILE* logFile = NULL;
static pthread_mutex_t mut;

bool log_open(char* pathArg)
{
    char* desc = "date,time,\"source host\",\"source port\",command,\"address type\",\"destination host\",\"destination port\"\n";
    char* path = "";

    if(path != NULL) path = pathArg;

    logFile = fopen(strcat(path, "log.csv"), "a+");
    if(logFile != NULL)
    {
        fseek(logFile, 0, SEEK_END);
        fputs(desc, logFile);
        fflush(logFile);
        return TRUE;
    }

    return FALSE;
}


void log_close()
{
    if(logFile != NULL) fclose(logFile);
}

void log_write(const log_entry_t *entry)
{
    char date[DATE_LEN + 1] = {0};
    get_date(date);

    char time[TIME_LEN + 1] = {0};
    get_time(time);

    char str[BUFSIZE + 1] = {0};

    strcat(str, date);
    strcat(str, ",");
    strcat(str, time);
    strcat(str, ",");
    strcat(str, entry->srcHost);
    strcat(str, ",");
    strcat(str, entry->srcPort);
    strcat(str, ",");
    strcat(str, entry->command);
    strcat(str, ",");
    strcat(str, entry->addrType);
    strcat(str, ",");
    strcat(str, entry->dstHost);
    strcat(str, ",");
    strcat(str, entry->dstPort);
    strcat(str, "\n");

    pthread_mutex_lock(&mut);
    fputs(str, logFile);
    fflush(logFile);
    pthread_mutex_unlock(&mut);
}

