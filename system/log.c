#include "log.h"
#include "misc/defs.h"
#include "misc/utils.h"
#include "misc/chrono.h"


#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

static FILE* logFile = NULL;
static pthread_mutex_t log_mutex;


void log_write(const char* host, const char* port, const char* cmd, const char* dstHost, const char* dstPort)
{
    char date[MAX_DATE_LEN + 1];
    get_date(date, MAX_DATE_LEN + 1);

    char time[MAX_TIME_LEN + 1];
    get_time(time, MAX_TIME_LEN + 1);

    char record[MAX_LOG_RECORD_LEN + 1];

    strcpy(record, date);
    strcat(record, ",");
    strcat(record, time);
    strcat(record, ",");
    strcat(record, host);
    strcat(record, ",");
    strcat(record, port);
    strcat(record, ",");
    strcat(record, cmd);
    strcat(record, ",");
    strcat(record, dstHost);
    strcat(record, ",");
    strcat(record, dstPort);

    strcat(record, "\n");

    pthread_mutex_lock(&log_mutex);
    fputs(record, logFile);
    pthread_mutex_unlock(&log_mutex);
}

bool log_open()
{
    const char* path = "/var/log/ace/";

    mkdir(path, S_IFDIR | S_IRWXU | S_IRWXO);

    char chrono[MAX_CHRONO_LEN + 1]; get_chrono(chrono);
    const char* extension = ".log";

    char logName[MAX_LOG_NAME_LEN + 1];
    strcpy(logName, path);
    strcat(logName, chrono);
    strcat(logName, extension);

    logFile = fopen(logName, "w+");

    return logFile != NULL;
}

void log_save()
{
  fflush(logFile);
}

void log_close()
{
    fclose(logFile);
}

