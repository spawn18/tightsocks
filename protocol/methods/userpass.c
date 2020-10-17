
#include "network/io.h"
#include "system/options.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define AUTH_LEN 513


bool handle_method_userpass(fd_t client)
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


        FILE* file = fopen("users.txt", "r");

        if(file != NULL)
        {
            char entry[512];

            while(fgets(entry, 512, file) != NULL)
            {
                if(strncmp(entry[0], user, userLen) == 0)
                {
                    if(strncmp(entry[userLen+1], pass, passLen) == 0)
                    {
                        success = TRUE;
                        break;
                    }
                }
            }

            fclose(file);
        }
    }

    char code = success ? 0 : 1;
    char rep[2] = {SOCKS_VER, code};
    send_all(client, rep, 2);

    return success;
}




