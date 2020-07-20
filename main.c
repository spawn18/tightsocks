
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
#include "include/utils.h"



/*
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
*/

/**
 * We call this for every thread, here goes the client <-> server conversation
 *
 * @param client client socket that we're connected with
 * @return doesn't return
 */
static void* process_client(void* arg)

{
    fd_t client = *(fd_t*)arg;

    /* Exchange method and get the chosen method */
    method_t method;
    exchange_methods(client, &method);

    /* Method specific sub-negotiation */
    if(method == METHODS_USERPASS) method_userpass(client);

    socks_request_t request;
    socks_get_request(client, request);

    socks_request_f request_func = NULL;
    if      (request[1] == COMMAND_CONNECT) request_func = &request_connect;
    else if (request[1] == COMMAND_BIND)    request_func = &request_bind;
    else                                    request_func = &request_udp_associate;

    /*
    request_func(&request[3]);


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
    printf("-c --max-clients=K     K is the amount of users that server is going to serve [default: 10]\n");
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
    char program_name[strlen(argv[0])+1];
    memcpy(program_name, argv[0], strlen(argv[0])+1);

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
                set_option(OPT_IPV4);
                break;
            }
            case '6':
            {
                set_option(OPT_IPV6);
                break;
            }
            case 'd':
            {
                if(!is_option_set(OPT_VERBOSE))
                {
                    set_option(OPT_DAEMON);
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
                if(!is_option_set(OPT_DAEMON))
                {
                    set_option(OPT_VERBOSE);
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
                set_option(OPT_MAX_CLIENTS);

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
    if(!is_option_set(OPT_VERBOSE) && !is_option_set(OPT_DAEMON))
    {
        set_option(OPT_VERBOSE);
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --verbose");
    }

    if(!is_option_set(OPT_IPV4) && !is_option_set(FLAGS & OPT_IPV6))
    {
        set_option(OPT_IPV4);
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --ipv4");
    }

    if(!is_option_set(OPT_MAX_CLIENTS))
    {
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT ARGUMENT FOR OPTION --max-client=10 ");
    }




    /* Нам нужна getaddrinfo для бинда(так как сервер может быть ipv4 или 6, и проше будет если geatddrinfo
     * заполнит всё */

    struct addrinfo hints = {0};
    struct addrinfo *results, *p_res;


    hints.ai_family = (is_option_set(OPT_IPV4)) ? AF_INET : AF_INET6;
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
    if(listen(listen_sock, (int)MAX_CLIENTS) == 0)
    {
        message(MSGTYPE_OK, "LISTEN() SUCCEED");
    }
    else
    {
        message(MSGTYPE_ERROR, "LISTEN() FAILED. TERMINATING...");
        exit(-1);
    }

    size_t total_clients = 0;
    while(total_clients < MAX_CLIENTS)
    {
        pthread_t thread = 0;
        pthread_attr_t thread_attr;

        /* Accept() requires us to have sockaddr struct to store info
         * about the connectee */
        struct sockaddr_storage ss;
        socklen_t ss_size = sizeof(*(struct sockaddr*)&ss);

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
            message(MSGTYPE_WARNING, "ACCEPT() FAILED");
            continue;
        }

        /* If we've connected to the client, then start a thread proccessing him there only */
        pthread_attr_init(&thread_attr);
        if(pthread_create(&thread, &thread_attr, process_client, &client_sock) != 0) return EXIT_FAILURE;
        pthread_detach(thread);

        ++total_clients;
    }

    return EXIT_SUCCESS;
}
