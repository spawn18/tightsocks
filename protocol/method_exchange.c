#include <protocol/methods/userpass.h>
#include <stdio.h>
#include "method_exchange.h"
#include "network/io.h"
#include "system/options.h"



bool SOCKS_handle_method(socket_t client)
{
    char buf[BUFSIZE + 1];

    if(recv_all(client, buf, 2) > 0)
    {
        if(buf[0] == SOCKS_VER && buf[1] != 0)
        {
            if(recv_all(client, buf+2, buf[1]) > 0)
            {
                method_t metPref = is_opt_set(OPT_USERPASS) ? MET_USERPASS : MET_NOAUTH;
                method_t metFinal = MET_NOMETHOD;

                for (int i = 0; i < buf[1]; i++)
                {
                    if (buf[2+i] == metPref)
                    {
                        metFinal = metPref;
                        break;
                    }
                }

                char rep[2] = {SOCKS_VER, metFinal};
                send_all(client, rep, 2);

                if(metFinal != MET_NOMETHOD)
                {
                    if(metFinal == MET_NOAUTH)
                    {
                        return TRUE;
                    }
                    if(metFinal == MET_USERPASS)
                    {
                        return method_userpass(client);
                    }
                }
            }
        }
    }

    return FALSE;
}