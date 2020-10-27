#include "log.h"
#include "misc/utils.h"
#include "misc/defs.h"
#include "misc/chrono.h"

#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

static FILE* logFile = NULL;
static pthread_mutex_t mut;

bool log_open()
{
    char* desc = "date,time,\"source host\",\"source port\",command,\"destination host\",\"destination port\"\n";

    logFile = fopen("log.csv", "r+");

    if(logFile == NULL)
    {
        logFile = fopen("log.csv", "a");
        if(logFile == NULL) return FALSE;
        fputs(desc, logFile);
        fflush(logFile);
    }
    else
    {
        fseek(logFile, 0, SEEK_END);
    }

    return TRUE;
}


void log_close()
{
    if(logFile != NULL)
    {
        fclose(logFile);
    }
}


void log_fmt_entry(struct sockaddr_storage srcAddr, char* req, log_entry_t* entry)
{
    fmt_addr(srcAddr, entry->srcHost, entry->srcPort);

    if(req[1] == CMD_CONNECT)
    {
        strcpy(entry->command, "CONNECT");
    }
    else if(req[2] == CMD_BIND)
    {
        strcpy(entry->command, "BIND");
    }
    else
    {
        strcpy(entry->command, "\"UDP ASSOCIATE\"");
    }

    atyp_t atyp;
    char *host;
    char *port;

    parse_req(req, &atyp, &host, &port);


    if(atyp == ATYP_DOMAINNAME)
    {
        strcpy(entry->addrType, "DOMAINNAME");
        strncpy(entry->dstHost, host+1, *host);
        entry->dstHost[*host] = '\0';
    }
    else
    {
        struct sockaddr_storage dstAddr;

        if(atyp == ATYP_IPV4)
        {
            strcpy(entry->addrType, "IPV4");

            dstAddr.ss_family = AF_INET;
            memcpy(&((struct sockaddr_in*)&dstAddr)->sin_addr.s_addr, host, 4);
            memcpy(&((struct sockaddr_in*)&dstAddr)->sin_port, port, 2);
            fmt_addr(dstAddr, entry->dstHost, entry->dstPort);
        }
        else if(atyp == ATYP_IPV6)
        {
            strcpy(entry->addrType, "IPV6");

            dstAddr.ss_family = AF_INET6;
            memcpy(&((struct sockaddr_in6*)&dstAddr)->sin6_addr.s6_addr, host, 4);
            memcpy(&((struct sockaddr_in6*)&dstAddr)->sin6_port, port, 2);
            fmt_addr(dstAddr, entry->dstHost, entry->dstPort);
        }
    }
}


void log_write(log_entry_t entry)
{
    char date[DATE_LEN + 1];
    get_date(date);

    char time[TIME_LEN + 1];
    get_time(time);

    char s[LOG_ENTRY_LEN + 1];

    strcat(s, date);
    strcat(s, ",");
    strcat(s, time);
    strcat(s, ",");
    strcat(s, entry.srcHost);
    strcat(s, ",");
    strcat(s, entry.srcPort);
    strcat(s, ",");
    strcat(s, entry.command);
    strcat(s, ",");
    strcat(s, entry.addrType);
    strcat(s, ",");
    strcat(s, entry.dstHost);
    strcat(s, ",");
    strcat(s, entry.dstPort);
    strcat(s, "\n");

    pthread_mutex_lock(&mut);
    fputs(s, logFile);
    fflush(logFile);
    pthread_mutex_unlock(&mut);
}

