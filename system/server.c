
#include "server.h"

#include "misc/defs.h"
#include "misc/utils.h"
#include "protocol/commands.h"
#include "protocol/request.h"
#include "system/log.h"
#include "misc/wrappers.h"
#include "system/options.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>


static int totalConns = 0;
static pthread_mutex_t mut;



static void* handle_client(void* arg)
{
    fd_t client = *(fd_t*)arg;

    if(SOCKS_exchange_methods(client))
    {
        bool methodHandled = FALSE;
        switch(METHOD_PREFERED)
        {
            case METHOD_NOAUTH:
                methodHandled = TRUE;
                break;

            case METHOD_USERPASS:
                methodHandled = handle_method_userpass(client);
                break;
        }

        if(methodHandled)
        {
            char req[REQ_LEN + 1];
            if(SOCKS_get_request(client, req))
            {
                char addrType = req[3];
                char* host = &req[3];
                char* port = (req[3] == ATYP_DOMAINNAME) ? &req[3+req[4]] :
                             (req[3] == ATYP_IPV4) ? &req[4] : &req[18];

                if (req[1] == CMD_CONNECT)   SOCKS_connect(client, addrType, host, port);
                else if (req[1] == CMD_BIND) SOCKS_bind(client, addrType, host, port);
                else                         SOCKS_udp_associate(client, addrType, host, port);
            }
        }
    }

    close(client);

    pthread_mutex_lock(&mut);
    --totalConns;
    pthread_mutex_unlock(&mut);

    pthread_exit(0);
}

void serve()
{
    fd_t server = socket((is_opt_set(OPT_IP4)) ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    if(server != -1)
    {
        printf("Created socket");

        struct sockaddr_storage addr;

        if(is_opt_set(OPT_IP4))
        {
            addr.ss_family = AF_INET;
            ((struct sockaddr_in*)&addr)->sin_addr.s_addr = htonl(INADDR_ANY);
            ((struct sockaddr_in*)&addr)->sin_port = htons(PORT);
        }
        else
        {
            addr.ss_family = AF_INET6;
            ((struct sockaddr_in6*)&addr)->sin6_addr = in6addr_any;
            ((struct sockaddr_in6*)&addr)->sin6_port = htons(PORT);
        }

        if(bind(server, (struct sockaddr*)&addr, sizeof(addr)) != -1)
        {
            printf("Bound address");

            if(listen(server, MAX_CONNECTIONS))
            {
                printf("Listening...");

                while(1)
                {
                    if (totalConns > MAX_CONNECTIONS)
                    {
                        continue;
                    }

                    int client = accept(server, NULL, NULL);

                    if (client != -1)
                    {
                        pthread_mutex_lock(&mut);
                        ++totalConns;
                        pthread_mutex_unlock(&mut);
                    }
                    else continue;

                    pthread_t thread = 0;
                    if (pthread_create(&thread, NULL, handle_client, &client) == 0)
                    {
                        pthread_detach(thread);
                    }
                }
            }
        }
    }
    else exit(0);
}