#ifndef ACE_CHRONO_H
#define ACE_CHRONO_H

#include "utils.h"

#define MAX_TIME_LEN 5
#define MAX_DATE_LEN 15

bool get_date(char* date, const size_t size);
bool get_time(char* time, const size_t size);
void get_chrono(char* chrono)

#endif 
