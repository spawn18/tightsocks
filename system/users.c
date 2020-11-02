#include "users.h"

#include <stdlib.h>

bool users_init()
{
    if(users == NULL)
    {
        usersLen = 10;
        users = malloc(sizeof(user_t) * usersLen);
        return TRUE;
    }

    return FALSE;
}

void users_add(const user_t *u)
{
    static int i = 0;

    if(i == usersLen)
    {
        usersLen *= 2;
        users = realloc(users, usersLen);
    }

    users[i] = *u;
    i++;
}

bool users_find(const user_t *u)
{
    for(int i = 0; i < usersLen; i++)
    {
        if(users[i].username == u->username && users[i].password == u->password)
        {
            return TRUE;
        }
    }

    return FALSE;
}