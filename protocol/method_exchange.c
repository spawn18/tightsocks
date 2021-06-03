#include "method_exchange.h"
#include "network/io.h"
#include "system/options.h"

#include <string.h>

bool SOCKS_handle_method(sock_t client)
{
    char buf[257];

    if(recv_all(client, buf, 2) > 0)
    {
        if(buf[0] == SOCKS_VER && buf[1] != 0)
        {
            if(recv_all(client, buf+2, buf[1]) > 0)
            {
                method_t metFinal = MET_NOMETHOD;

                for (int i = 0; i < buf[1]; i++)
                {
                    if (buf[2+i] == MET_NOAUTH)
                    {
                        metFinal = MET_NOAUTH;
                        break;
                    }
                    else if (buf[2+i] == MET_USERPASS)
                    {
                        if(IS_OPT_SET(OPT_AUTH))
                        {
                            metFinal = MET_USERPASS;
                            break;
                        }
                    }
                }

                char rep[2] = {SOCKS_VER, metFinal};
                send_all(client, rep, 2);

                if (metFinal == MET_NOAUTH) 
                    return TRUE;
                else if (metFinal == MET_USERPASS) 
                    return auth_userpass(client);
            }
        }
    }

    return FALSE;
}


bool auth_userpass(sock_t client)
{
    // Single buffer to avoid unneccesary recv calls
    char buf[513];

    // Get version and username length
    if(recv_all(client, buf, 2) > 0)
    {
        // Check version and username length
        if(buf[0] == SOCKS_VER && buf[1] != 0)
        {
            // Get username
            if(recv_all(client, buf+2, buf[1]) > 0)
            {
                // Get password length
                if(recv_all(client, buf+2+buf[1], 1) > 0)
                {
                    // Get password
                    if(recv_all(client, buf+3+buf[1], buf[1+buf[1]]) > 0)
                    {
                        // Check username
                        if( (USERNAME_LEN == buf[1] && strncmp(USERNAME, buf+2, USERNAME_LEN) == 0) &&
                            (PASSWORD_LEN == buf[1+buf[1]] && strncmp(PASSWORD, buf+3+buf[1], PASSWORD_LEN) == 0) )
                        {
                            char rep[2] = {SOCKS_VER, 0};
                            send_all(client, rep, 2);

                            return TRUE;
                        }
                        else 
                        {
                            char rep[2] = {SOCKS_VER, 1};
                            send_all(client, rep, 2);

                            return FALSE;
                        }
                    }
                }
            }   
        }
    }

    return FALSE;
}