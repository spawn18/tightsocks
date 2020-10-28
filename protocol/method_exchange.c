#include "method_exchange.h"
#include "network/io.h"
#include "request.h"
#include "system/options.h"



bool SOCKS_exchange_methods(fd_t client)
{
    char methods[METHODS_LEN + 1];

    if(recv_all(client, methods, 2) > 0)
    {
        if(methods[0] == SOCKS_VER && methods[1] != 0)
        {
            if(recv_all(client, methods+2, methods[1]) > 0)
            {
                method_t m = METHOD_NOMETHOD;

                for (int i = 0; i < methods[1]; i++)
                {
                    if (methods[2+i] == METHOD_PREFERED)
                    {
                        m = METHOD_PREFERED;
                        break;
                    }
                }

                char rep[2] = {SOCKS_VER, (char)m};
                send_all(client, rep, 2);

                if(m != METHOD_NOMETHOD) return TRUE;
                else return FALSE;
            }
        }
    }

    return FALSE;
}