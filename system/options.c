#include "options.h"

#include <stddef.h>

unsigned short PORT = 1080;
int MAX_CONNS = 1024;
int	BUFSIZE = 1 << 16;
char* USERNAME = NULL;
char* PASSWORD = NULL;
int USERNAME_LEN = 0;
int PASSWORD_LEN = 0;
option_t OPTIONS = 0;

