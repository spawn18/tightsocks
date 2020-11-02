#ifndef ACE_USERS_H
#define ACE_USERS_H

#include <stddef.h>
#include <misc/defs.h>
#include <stdlib.h>

#define USERNAME_LEN 255
#define PASSWORD_LEN 255

typedef struct USER_T
{
    char username[USERNAME_LEN + 1];
    char password[PASSWORD_LEN + 1];
} user_t;

static int usersLen = 0;
static user_t* users = NULL;

bool users_init();
void users_add(const user_t *u);
bool users_find(const user_t *u);

#endif 
