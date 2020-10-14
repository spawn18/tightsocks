
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



static size_t totalConnections = 0;
static pthread_mutex_t totalConnections_mutex;

size_t get_conns()
{
    return totalConnections;
}

void inc_conns()
{
    pthread_mutex_lock(&totalConnections_mutex);
    ++totalConnections;
    pthread_mutex_unlock(&totalConnections_mutex);
}

void dec_conns()
{
    pthread_mutex_lock(&totalConnections_mutex);
    --totalConnections;
    pthread_mutex_unlock(&totalConnections_mutex);
}


/**
 * @brief Used to serve a single client
 * 
 * Here goes the standard procedure for every individual client
 * 
 * @param arg Socket descriptor that is connected and that describes a client
 * 
 * @return Nothing because the method is always used in detach state.
 */
static void* handle_client(void* arg)
{
    fd_t client = *(fd_t*)arg;

    if(SOCKS_exchange_methods(client, METHOD_PREFERED))
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
    dec_conns();
    pthread_exit(0);
}

void serve()
{
    if(is_opt_set(OPT_LOG) && !log_open())
    {
        MSG("Can't open log file");
        exit(-1);
    }

    char* host = NULL;
    char* port = "1080";
    int family     = AF_INET;
    int socketType = SOCK_STREAM;
    int protocol   = 0;
    int flags = AI_PASSIVE;

    struct addrinfo *results = NULL;

    if(find_sockets(host, port, family, socketType, protocol, flags, results))
    {
        MSG("Found sockets to bind");

        fd_t server;
        if(bind_socket(&server, results))
        {
            MSG("Bound port");

            if(listen(server, MAX_CONNECTIONS) == 0)
            {
                MSG("Listening for connections \n Server has started. Let's go!");

                while(1)
                {
                    if(get_conns() > MAX_CONNECTIONS)
                    {
                        continue;
                    }

                    int client = accept(server, NULL, NULL);

                    if(client != -1) inc_conns();
                    else continue;

                    pthread_t thread = 0;
                    if(pthread_create(&thread, NULL, handle_client, &client) == 0)
                    {
                        pthread_detach(thread);
                    }
                }
            }
        }
    }
}