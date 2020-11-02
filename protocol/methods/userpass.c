#include "network/io.h"
#include "system/users.h"

#include <stdio.h>
#include <string.h>


bool handle_method_userpass(socket_t client)
{
    char auth[AUTH_LEN + 1];
    recv_all(client, auth, 2);

    bool success = FALSE;

    if(auth[0] == SOCKS_VER)
    {
        recv_all(client, auth+2, auth[1]+1);

        char* user = auth+2;
        char userLen = auth[1];

        recv_all(client, auth+3+userLen, auth[2+userLen]);
        char* pass = auth+3+userLen;
        char passLen = auth[2+userLen];

        user_t u = {'\0'};
        strncpy(u.username, user, userLen);
        strncpy(u.password, pass, passLen);

        if(users_find(&u)) success = TRUE;
    }

    char code = success ? 0 : 1;
    char rep[2] = {SOCKS_VER, code};
    send_all(client, rep, 2);

    return success;
}




