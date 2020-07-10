#include "../include/message.h"

#include <stdio.h>

void message(enum MSGTYPE msgtype, char* msg)
{
    char* code = 0;
    switch((int)msgtype)
    {
        case MSGTYPE_ERROR:
            fprintf(stderr,"[ ERROR ]  %s\n", msg);
            return;
        case MSGTYPE_OK:
            code = "OK";
            break;
        case MSGTYPE_WARNING:
            code = "WARNING";
            break;
        case MSGTYPE_INFO:
            code = "INFO";
            break;
    }

    fprintf(stdout,"[ %s ]  %s\n", code, msg);

}
