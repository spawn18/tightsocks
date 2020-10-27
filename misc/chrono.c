#include "chrono.h"
#include "defs.h"

#include <time.h>
#include <string.h>




void get_date(char* buf)
{
    time_t t = time(NULL);
    struct tm *tms = NULL;
    tms = localtime(&t);
    strftime(buf, DATE_LEN, "%d/%m/%Y", tms);
}


void get_time(char* buf)
{
    time_t t = time(NULL);
    struct tm *tms = NULL;
    tms = localtime(&t);
    strftime(buf, TIME_LEN, "%T", tms);
}

