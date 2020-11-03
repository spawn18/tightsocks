#include "userpass.h"

#include "network/io.h"
#include "system/users.h"

#include <stdio.h>
#include <string.h>


bool method_userpass(socket_t client)
{
    char buf[BUFSIZE + 1];

    if(recv_all(client, buf, 2) > 0)
    {
        if(buf[0] == SOCKS_VER)
        {
            if(recv_all(client, buf+2, buf[1]+1) > 0)
            {
                char* user = buf+2;
                char userLen = buf[1];

                if(recv_all(client, buf+3+userLen, buf[2+userLen]) > 0)
                {
                    char* pass = buf+3+userLen;
                    char passLen = buf[2+userLen];

                    user_t u = {'\0'};
                    strncpy(u.username, user, userLen);
                    strncpy(u.password, pass, passLen);

                    if(users_find(&u))
                    {
                        char rep[2] = {SOCKS_VER, 0};
                        send_all(client, rep, 2);

                        return TRUE;
                    }
                    else
                    {
                        char rep[2] = {SOCKS_VER, 1};
                        send_all(client, rep, 2);
                    }
                }
            }
        }
    }

    return FALSE;
}




