#include "chrono.h"

#include <time.h>
#include <string.h>



/**
 * @brief Get date in day/month/year format
 *
 * @param [out] dateOut Buffer to write date to
 * @param [out] dateOutSize Size of given buffer
 *
 * @return TRUE if success, FALSE otherwise
 */
bool get_date(char* date, const size_t size)
{
    time_t currentTime = time(NULL);
    if(currentTime != -1)
    {
        struct tm *timeStruct = NULL;
        timeStruct = localtime(&currentTime);
        if(timeStruct != NULL)
        {
            const char* format = "%d/%m/%Y";
            size_t r = strftime(date, size, format, timeStruct);
            if(r == 0) TRUE;
        }
    }

    return FALSE;
}


/**
 * @brief Get time in format minutes:hours
 *
 * @param time Buffer to store time
 * @param size Size of given buffer
 *
 * @return TRUE if success, FALSE otherwise
 */
bool get_time(char* time_, const size_t size)
{
    time_t curTime = time(NULL);
    if(curTime != -1)
    {
        struct tm *timeStruct = NULL;
        timeStruct = localtime(&curTime);
        if(timeStruct != NULL)
        {
            const char* format = "%R";
            size_t r = strftime(time_, size, format, timeStruct);
            if(r == 0) TRUE;
        }
    }

    return FALSE;
}

