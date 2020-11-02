#include "server.h"

#include "misc/defs.h"
#include "protocol/request.h"
#include "log.h"
#include "protocol/commands/connect.h"
#include "protocol/commands/bind.h"
#include "protocol/commands/udp_associate.h"
#include "system/options.h"


#include <stdlib.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>


static int totalConns = 0;
static pthread_mutex_t mut;


void* handle_client(void* arg)
{
    socket_t client = *(socket_t*)arg;

    if(SOCKS_exchange_methods(client))
    {
        if(is_opt_set(OPT_USERPASS))
        {
            handle_method_userpass(client);
        }

        if(methodHandled)
        {
            request_t req;
            if(SOCKS_handle_request(client, &req))
            {
                if(is_opt_set(OPT_LOG))
                {
                    struct sockaddr_storage addr;
                    socklen_t len = sizeof(addr);

                    if(getpeername(client, (struct sockaddr*)&addr, &len) == 0)
                    {
                        log_entry_t entry;
                        log_fmt_entry(&addr, &req, &entry);
                        log_write(&entry);
                    }
                }

                if (req.CMD == CMD_CONNECT)   SOCKS_connect(client, &req);
                else if (req.CMD == CMD_BIND) SOCKS_bind(client, &req);
                else                          SOCKS_udp_associate(client, &req);
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
    socket_t server4;
    socket_t server6;

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