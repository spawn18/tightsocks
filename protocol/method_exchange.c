#include "method_exchange.h"
#include "network/io.h"



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
                    if (buf[2 + i] == MET_NOAUTH)
                    {
                        metFinal = MET_NOAUTH;
                        break;
                    }
                }

                char rep[2] = {SOCKS_VER, metFinal};
                send_all(client, rep, 2);

                if (metFinal == MET_NOAUTH) return TRUE;
            }
        }
    }

    return FALSE;
}