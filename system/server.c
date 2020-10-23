
#include "server.h"

#include "misc/defs.h"
#include "protocol/commands.h"
#include "protocol/request.h"
#include "system/options.h"


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>


static int totalConns = 0;
static pthread_mutex_t mut;


void* handle_client(void* arg)
{
    fd_t client = *(fd_t*)arg;

    if(SOCKS_exchange_methods(client))
    {
        printf("1\n");

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
                printf("2\n");

                atyp_t atyp = req[3];
                char* host = &req[4];
                char* port = (req[3] == ATYP_DOMAINNAME) ? &req[3+req[4]] : (req[3] == ATYP_IPV4) ? &req[8] : &req[20];

                if (req[1] == CMD_CONNECT)   SOCKS_connect(client, atyp, host, port);
                else if (req[1] == CMD_BIND) SOCKS_bind(client, atyp, host, port);
                else                         SOCKS_udp_associate(client, atyp, host, port);
            }
        }
    }

    close(client);

    pthread_mutex_lock(&mut);
    --totalConns;
    pthread_mutex_unlock(&mut);

    free((int*)arg);

    pthread_exit(0);
}


void serve()
{
    fd_t server4;
    fd_t server6;

    if(is_opt_set(OPT_IP4))
    {
        server4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(server4 == -1)
        {
            exit(-1);
        }

        struct sockaddr_in addr4;
        addr4.sin_family = AF_INET;
        addr4.sin_addr.s_addr = htonl(INADDR_ANY);
        addr4.sin_port = htons(PORT);

        if(bind(server4, (struct sockaddr*)&addr4, sizeof(addr4)) == -1)
        {
            exit(-1);
        }

        if(listen(server4, MAX_CONNECTIONS) == -1)
        {
            exit(-1);
        }

        fcntl(server4, F_SETFL, O_NONBLOCK);
    }


    if(is_opt_set(OPT_IP6))
    {
        server6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

        if(server6 == -1)
        {
            exit(-1);
        }

        struct sockaddr_in6 addr6;
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(PORT);

        if(bind(server6, (struct sockaddr*)&addr6, sizeof(addr6)) == -1)
        {
            exit(-1);
        }

        if(listen(server6, MAX_CONNECTIONS) == -1)
        {
            exit(-1);
        }

        fcntl(server6, F_SETFL, O_NONBLOCK);
        int on = 1;
        setsockopt(server6, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&on, sizeof(on));
    }

    while(1)
    {
        if (totalConns > MAX_CONNECTIONS)
        {
            continue;
        }

        if(is_opt_set(OPT_IP4))
        {
            int client = accept(server4, NULL, NULL);
            if(client != -1)
            {
                pthread_mutex_lock(&mut);
                ++totalConns;
                pthread_mutex_unlock(&mut);

                pthread_t thread = 0;
                int* clientArg = malloc(sizeof(client));
                *clientArg = client;
                if (pthread_create(&thread, NULL, handle_client, clientArg) == 0)
                {
                    pthread_detach(thread);
                }
            }
        }

        if(is_opt_set(OPT_IP6))
        {
            int client = accept(server6, NULL, NULL);
            if(client != -1)
            {
                pthread_mutex_lock(&mut);
                ++totalConns;
                pthread_mutex_unlock(&mut);

                pthread_t thread = 0;
                int* clientArg = malloc(sizeof(client));
                *clientArg = client;
                if (pthread_create(&thread, NULL, handle_client, clientArg) == 0)
                {
                    pthread_detach(thread);
                }
            }
        }
    }
}