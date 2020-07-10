
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>

#include "include/types.h"
#include "include/io.h"
#include "include/convert.h"
#include "include/dnslookup.h"
#include "include/socks.h"
#include "include/message.h"
#include "include/methods.h"


/*
void get_sockaddr(struct sockaddr_storage* client)
{
    char s[INET6_ADDRSTRLEN];

    inet_ntop(client->ss_family, get_sockaddr_address(&(*client)), s, sizeof(s));
    in_port_t port = ntohs(get_sockaddr_port(&(*client)));

    printf("Got connection from ip %s and port %d \n\r", s, port);
}
*/


static void message_loop(fd_t client, fd_t server)
{
    struct pollfd fd[2];

    fd[0].fd = client;
    fd[0].events = POLLIN | POLLOUT;

    fd[1].fd = server;
    fd[1].fd = POLLIN | POLLOUT;

    do
    {
        res_t r = poll(fd, 2, 3000);
        if(r == -1) return;
        if(r == 0) return;

	char client_buf[BUFIZE];
	char server_buf[BUFSIZE];

        if(fd[0].revents & POLLIN)
        {
            char buf[BUFSIZE];
            if(recvf(fd[0].fd, buf, sizeof(buf)) == -1) return;

            if(fd[1].revents & POLLOUT) sendf(fd[1].fd, buf, sizeof(buf));
        }

        if(fd[1].revents & POLLIN)
        {
            char buf[BUFSIZE];
            if(recvf(fd[1].fd, buf, sizeof(buf)) == -1) return;

            if(fd[0].revents & POLLOUT) sendf(fd[0].fd, buf, sizeof(buf));
        }

    }
    while(1);
}

/**
 * We call this for every thread, here goes the client <-> server conversation
 *
 * @param client client socket that we're connected with
 * @return doesn't return
 */
static void* process_client(void* arg)
{
    fd_t client = (fd_t)arg;


    /* Exchange method and get the chosen method */
    method_t method = exchange_methods(client);

    /* Method specific sub-negotiation */
    switch(method)
    {
        case METHODS_VAR_USERPASS:
        {
            method_userpass(client);
            break;
        }
        default:
            break;
    }


    /* struct to contain user's ip and port*/
    struct sockaddr_storage ss;

    /* We get user request and read target address and port to struct */
    res_t req_type = evaluate_request(client, &ss);
    if(req_type == -1)
    {
        if(FLAGS & OPT_VERBOSE) message(MSGTYPE_ERROR, "INCORRECT REQUEST");
        goto exit;
    }

    switch(req_type)
    {
        case REQTYPE_CONNECT:
        {
            fd_t server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(server_sock != 0)
            {
                if(FLAGS & OPT_VERBOSE) message(MSGTYPE_ERROR, "COULDN'T CREATE A SOCKET");
                goto exit;
            }

            if(connect( server_sock, (struct sockaddr*)&ss, sizeof( *((struct sockaddr*)&ss) ) ) != 0)
            {
                message(MSGTYPE_ERROR, "COULDN'T ESTABLISH CONNECTION TO PEER");
                goto exit;
            }

            message_loop(client_sock, server_sock);

            break;
        }
        case REQTYPE_BIND:
        {
            //r = socks5_bind_req(address, sizeof(address), port);

            break;
        }
        case REQTYPE_UDP_ASSOCIATE:
        {
            //r = socks5_udpassoc_req(address, sizeof(address), port);

            break;
        }
        default:
            goto exit;
    }

    exit:
        shutdown(client, SHUT_RDWR);
        close(client);
        pthread_exit(0);
}

void usage(char* name)
{
    printf("Usage: %s [ -4 | -6 {-v | -d} (-c=K) ]  \n", name);
    printf("Usage: %s --help \n", name);
    printf(" \n");
    printf("-4 --ipv4        Accept/handle ip version 4 connections [default]\n");
    printf("-6 --ipv6        Accept/handle ip version 6 connections \n");
    printf("-d --daemon      Run as daemon (background process) \n");
    printf("-v --verbose     Print server messages [default]\n");
    printf("-h --help        Print this usage guide - don't launch the program \n");
    printf("-c --max-clients=K     The K amount of users that server is going to serve [default: 1]\n");
}

struct option long_options[] =
        {
        {"ipv4", 0, 0, '4'},
        {"ipv6", 0, 0, '6'},
        {"daemon", 0, 0, 'd'},
        {"verbose", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {"max-clients", 1, 0, 'c'}
};


int main(int argc, char** argv)
{
    /* Used for help message, if user ever renames the executable
     * Since getopt changes args order and manipulates them we need to do a deep copy*/
    char program_name[strlen(argv[0]) + 1];
    memcpy(program_name, argv[0], strlen(argv[0]) + 1);

    /* Returned character for getopt_long*/
    int c = 0;

    /* We loop over command line options to configure the server
     * No options are required since we have some set by default */
    do
    {
        /* Get current option */
        c = getopt_long(argc, argv, "46dvhc:", long_options, NULL);

        /* Determine the option and set the flag accordingly */
        switch(c)
        {
            case 'h':
            {
                usage(program_name);
                return 0;
            }
            case '4':
            {
                FLAGS |= OPT_IPV4;
                break;
            }
            case '6':
            {
                FLAGS |= OPT_IPV6;
                break;
            }
            case 'd':
            {
                if(!(FLAGS & OPT_VERBOSE))
                {
                    FLAGS |= OPT_DAEMON;
                    break;
                }
                else
                {
                    message(MSGTYPE_ERROR, "INCORRECT OPTIONS");
                    return -1;
                }
            }
            case 'v':
            {
                if(!(FLAGS & OPT_DAEMON))
                {
                    FLAGS |= OPT_VERBOSE;
                    break;
                }
                else
                {
                    message(MSGTYPE_ERROR, "INCORRECT OPTIONS");
                    return -1;
                }
            }
            case 'c':
            {
                FLAGS |= OPT_MAX_CLIENTS;
                MAX_CLIENTS = strtol(optarg, NULL, 10);
                if(MAX_CLIENTS == 0L || errno == ERANGE)
                {
                    message(MSGTYPE_ERROR, "INCORRECT OPTIONS");
                }

                if (MAX_CLIENTS > 1024)
                    MAX_CLIENTS = 1024;

                break;
            }
            case '?': /* Error out if option is unrecognized or the argument is missed*/
            {
                message(MSGTYPE_ERROR, "INCORRECT OPTIONS");
                return -1;
            }
            default:
                break;
        }
    }
    while(c != -1);

    /* We need to set standard arguments if there weren't specified to configure program */
    if(!(FLAGS & OPT_VERBOSE) && !(FLAGS & OPT_DAEMON))
    {
        FLAGS |= OPT_VERBOSE;
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --verbose");
    }

    if(!(FLAGS & OPT_IPV4) && !(FLAGS & OPT_IPV6))
    {
        FLAGS |= OPT_IPV4;
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --ipv4");
    }

    if(!(FLAGS & OPT_MAX_CLIENTS))
    {
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT ARGUMENT FOR OPTION --max-client=10 ");
    }




    /* Нам нужна getaddrinfo для бинда(так как сервер может быть ipv4 или 6, и проше будет если geatddrinfo
     * заполнит всё */

    struct addrinfo hints = {0};
    struct addrinfo *results, *p_res;


    hints.ai_family = (FLAGS & OPT_IPV4) ? AF_INET : AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG | AI_PASSIVE;

    if(getaddrinfo(NULL, SOCKS_PORT, &hints, &results) != 0)
    {
        message(MSGTYPE_ERROR, "FAILED TO CREATE LISTENING SOCKET");
    }

    fd_t listen_sock = -1;
    for(p_res = results; p_res != NULL; p_res = p_res->ai_next)
    {
        listen_sock = socket(p_res->ai_family, p_res->ai_socktype, p_res->ai_protocol);
        if(listen_sock != 0)
        {
            continue;
        }

        if(bind(listen_sock, p_res->ai_addr, p_res->ai_addrlen) != -1)
        {
           break;
        }
        else
        {
            close(listen_sock);
        }
    }

    if(p_res == NULL)
    {
        message(MSGTYPE_ERROR, "NO SOCKETS AVAILABLE");
    }

    freeaddrinfo(results);

    /* If it doesnt work out - we terminate */
    if(listen(listen_sock, MAX_CLIENTS) == 0)
    {
        message(MSGTYPE_OK, "LISTEN() SUCCEED");
    }
    else
    {
        message(MSGTYPE_ERROR, "LISTEN() FAILED. TERMINATING...");
        exit(-1);
    }

    /* For performance purposes there's only max amount of 10
     * people being served. Just redefine if you need more */
    size_t total_clients = 0;
    while(total_clients < MAX_CLIENTS)
    {
        pthread_t thread = 0;
        pthread_attr_t thread_attr;

        /* Accept() requires us to have sockaddr struct to store info
         * about the connectee */
        struct sockaddr_storage ss;
        socklen_t ss_size = sizeof(struct sockaddr_storage);

        /* We accept the client */
        int client_sock = accept(listen_sock, (struct sockaddr *) &ss, &ss_size);

        /* Print info about the client we just connected */
        if(client_sock != -1) 
        {
            /* Ip and port in string */
            char ip[INET6_ADDRSTRLEN] = {0};
            char port[3] = {0};

            /* Convert them from int to char */
            inet_ntop(ss.ss_family, getaddrss(&ss), ip, sizeof(ip));
            sprintf(port, "%d", (int)getportss(&ss));

            /* Concatenate with the final message */
            char output[80] = {0};
            strcpy(output, "GOT CONNECTION FROM CLIENT ");
            strcat(output, ip);
            strcat(output, " ON PORT ");
            strcat(output, port);

            /* And message about the connectee */
            message(MSGTYPE_INFO, output);
        }
        else
        {
            message(MSGTYPE_ERROR, "ACCEPT() FAILED. TERMINATING...");
            return -1;
        }

        /* TODO: Если я правильно понял то вот с этой строчки кода у нас дальше айпи агностик идёт. Разберись */
        /* If we've connected to the client, then start a thread proccessing him there only */
        pthread_attr_init(&thread_attr);
        if(pthread_create(&thread, &thread_attr, process_client, &client_sock) == -1) return -1;
        pthread_detach(thread);

        ++total_clients;
    }

    return EXIT_SUCCESS;
}
