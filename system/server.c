#include "server.h"

#include "misc/defs.h"
#include "protocol/request.h"
#include "log.h"
#include "protocol/commands/connect.h"
#include "protocol/method_exchange.h"
#include "system/options.h"

#include <stdlib.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <misc/utils.h>
#include <stdio.h>
#include <errno.h>

static int totalConns = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* arg)
{
    sock_t client = *(sock_t*)arg;

    if(SOCKS_handle_method(client))
    {
        request_t req = {0};

        if(SOCKS_get_request(client, &req))
        {
            if(IS_OPT_SET(OPT_LOG))
            {
                struct sockaddr_storage addr = {0};
                socklen_t len = sizeof(addr);

                if(getpeername(client, (struct sockaddr*)&addr, &len) == 0)
                {
                    log_entry_t entry = {0};

                    hosttop(&addr, entry.srcHost, entry.srcPort);
                    cmdtop(req.CMD, entry.cmd);
                    atyptop(req.ATYP, entry.addrType);
                    reqhosttop(req.ATYP, req.DSTADDR, req.DSTPORT, entry.dstHost, entry.dstPort);

                    log_write(&entry);
                }
            }

            if(req.CMD == CMD_CONNECT)       
            {
              SOCKS_connect(client, req.ATYP, req.DSTADDR, req.DSTPORT);  
            }
            else if(req.CMD == CMD_BIND)
            {
                // unsupported
            }
            else
            {
                // unsupported
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

    sock_t serv, serv4, serv6;

    if(IS_OPT_SET(OPT_IP4))
    {
        serv4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        int on = 1;
        setsockopt(serv4, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(serv4 != -1)
        {
            INFO("Created IPv4 socket");
        }
        else
        {
            ERR("Failed to create IPv4 socket", strerror(errno));
            exit(-1);
        }

        struct sockaddr_in addr4 = {0};
        addr4.sin_family = AF_INET;
        addr4.sin_addr.s_addr = INADDR_ANY;
        addr4.sin_port = htons(PORT);

        if(bind(serv4, (struct sockaddr*)&addr4, sizeof(addr4)) == 0)
        {
            INFO("Bound address");
        }
        else
        {
            ERR("Failed to bind address", strerror(errno));
            exit(-1);
        }

        if(listen(serv4, MAX_CONNS) == 0)
        {
            INFO("Listening...");
        }
        else
        {
            ERR("Listen failed", strerror(errno));
            exit(-1);
        }

        if(fcntl(serv4, F_SETFL, O_NONBLOCK) != -1)
        {
           INFO("Socket switched to non-blocking mode");
        }
        else
        {
            ERR("Failed to switch socket to non-blocking mode", strerror(errno));
            exit(-1);
        }

        serv = serv4;
    }


    if(IS_OPT_SET(OPT_IP6))
    {
        serv6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

        int on = 1;
        setsockopt(serv6, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&on, sizeof(on));
        setsockopt(serv6, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(serv6 != -1)
        {
            INFO("Created IPv6 socket");
        }
        else
        {
            ERR("Failed to create IPv6 socket", strerror(errno));
            exit(-1);
        }

        struct sockaddr_in6 addr6 = {0};
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(PORT);

        if(bind(serv6, (struct sockaddr*)&addr6, sizeof(addr6)) == 0)
        {
            INFO("Bound address");
        }
        else
        {
            ERR("Failed to bind address", strerror(errno));
            exit(-1);
        }

        if(listen(serv6, MAX_CONNS) == 0)
        {
            INFO("Listening...");
        }
        else
        {
            ERR("Listen failed", strerror(errno));
            exit(-1);
        }

        if(fcntl(serv6, F_SETFL, O_NONBLOCK) != -1)
        {
            INFO("Socket switched to non-blocking mode");
        }
        else
        {
            ERR("Failed to switch socket to non-blocking mode", strerror(errno));
            exit(-1);
        }


        serv = serv6;
    }

    if(log_open()) 
    {
        INFO("Log opened");
    }
    else 
    {
        ERR("Log can't be open", strerror(errno));
        exit(-1);
    }

    INFO("Server started!");

    while(1)
    {
        if (totalConns > MAX_CONNS)
        {
            INFO("Maximum number of connections reached");
            continue;
        }

        int client = accept(serv, NULL, NULL);

        if(client != -1)
        {
            pthread_mutex_lock(&mut);
            ++totalConns;
            pthread_mutex_unlock(&mut);

            int *arg = malloc(sizeof(client));
            *arg = client;

            pthread_t t = 0;
            if (pthread_create(&t, NULL, handle_client, arg) == 0)
            {
                pthread_detach(t);
            }
            else
            {
                WARN("Failed to create thread for client");
            }
        }
        else
        {
            if(errno != EAGAIN || errno != EWOULDBLOCK)
            {
                WARN("Accept() failed");
            }
        }

        if(IS_OPT_SET(OPT_IP4) && IS_OPT_SET(OPT_IP6))
        {
            serv = (serv == serv4) ? serv6 : serv4;
        }
    }
}
