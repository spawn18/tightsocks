#include "method_exchange.h"
#include "network/io.h"
#include "request.h"
#include "system/log.h"
#include "system/options.h"



bool SOCKS_exchange_methods(fd_t client)
{
    char methods[METHODS_LEN + 1];

    if(recv_all(client, methods, 2))
    {
        if((methods[0] != SOCKS_VER) && (methods[1] != 0))
        {
            if(recv_all(client, methods+2, methods[1]))
            {
                char met = METHOD_NOMETHOD;

                for (int i = 2; i < methods[1]; i++)
                {
                    if (methods[i] == METHOD_PREFERED)
                    {
                        met = METHOD_PREFERED;
                        break;
                    }
                }

                if(met == METHOD_NOMETHOD)
                {
                   return FALSE;
                }

                char rep[2] = {SOCKS_VER, met};
                send_all(client, rep, 2);

                return TRUE;
            }
        }
    }

    return FALSE;
}