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

bool log_open(char* path)
{
    char* desc = "date,time,\"source host\",\"source port\",command,\"address type\",\"destination host\",\"destination port\"\n";

    if(path)
    {
        logFile = fopen(path, "r+");
    }
    else
    {
        logFile = fopen("log.csv", "r+");
    }


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


void log_fmt_entry(const struct sockaddr_storage *srcAddr, const request_t *req, log_entry_t* entry)
{
    unload_addr(srcAddr, entry->srcHost, entry->srcPort);

    if(req->CMD == CMD_CONNECT)
    {
        strcpy(entry->command, "CONNECT");
    }
    else if(req->CMD == CMD_BIND)
    {
        strcpy(entry->command, "BIND");
    }
    else
    {
        strcpy(entry->command, "\"UDP ASSOCIATE\"");
    }


    if(req->ATYP == ATYP_DOMAINNAME)
    {
        strcpy(entry->addrType, "DOMAINNAME");
        strncpy(entry->dstHost, &req->DSTADDR[1], req->DSTADDR[0]);
        entry->dstHost[req->DSTADDR[0]] = '\0';
    }
    else
    {
        struct sockaddr_storage dstAddr;

        if(req->ATYP == ATYP_IPV4)
        {
            strcpy(entry->addrType, "IPV4");

            dstAddr.ss_family = AF_INET;
            memcpy(&((struct sockaddr_in*)&dstAddr)->sin_addr.s_addr, req->DSTADDR, 4);
            memcpy(&((struct sockaddr_in*)&dstAddr)->sin_port, req->DSTPORT, 2);

            unload_addr(&dstAddr, entry->dstHost, entry->dstPort);
        }
        else if(req->ATYP == ATYP_IPV6)
        {
            strcpy(entry->addrType, "IPV6");

            dstAddr.ss_family = AF_INET6;
            memcpy(&((struct sockaddr_in6*)&dstAddr)->sin6_addr.s6_addr, req->DSTADDR, 16);
            memcpy(&((struct sockaddr_in6*)&dstAddr)->sin6_port, req->DSTPORT, 2);

            unload_addr(&dstAddr, entry->dstHost, entry->dstPort);
        }
    }
}


void log_write(const log_entry_t *entry)
{
    char date[DATE_LEN + 1] = {0};
    get_date(date);

    char time[TIME_LEN + 1] = {0};
    get_time(time);

    char str[BUFSIZE + 1];

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

