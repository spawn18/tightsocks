
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

#define BUFSIZE 256
#define SOCKS_PORT "1080"
#define SOCKS_VERSION 0x05



static unsigned char FLAGS = 0x0;
static long MAX_CLIENTS = 10;


void message(enum MSGTYPE msgtype, char* msg)
{
    if(FLAGS & BM_OPT_VERBOSE)
    {
        char* code = 0;
        switch((int)msgtype)
        {
            case MSGTYPE_ERROR:
                fprintf(stderr,"[ ERROR ]  %s\n", msg);
                return;
            case MSGTYPE_OK:
                code = "OK";
                break;
            case MSGTYPE_WARNING:
                code = "WARNING";
                break;
            case MSGTYPE_INFO:
                code = "INFO";
                break;
        }

        fprintf(stdout,"[ %s ]  %s\n", code, msg);
    }
}


/*
void get_sockaddr(struct sockaddr_storage* client)
{
    char s[INET6_ADDRSTRLEN];

    inet_ntop(client->ss_family, get_sockaddr_address(&(*client)), s, sizeof(s));
    in_port_t port = ntohs(get_sockaddr_port(&(*client)));

    printf("Got connection from ip %s and port %d \n\r", s, port);
}
*/


/* TODO: SPLIT THESE TO FILE */
/**
 * Receives client's supported methods and sends one back
 * If client supports no authentification we prefer it
 * Either way we send NO_METHOD back indicating that it was a bad negotiation
 * And terminate the connection
 *
 * @param client Socket to read from
 * @param buf Buffer that gets filled with the clients message
 * @param bufsize Buffer size
 *
 * @return 0 if success, -1 on error
 */
static res_t exchange_methods(fd_t client)
{
    /* Buffer to read METHOD selection message (maximum 257 bytes)*/
    char buffer[BUFSIZE + 1];

    /* Get first two bytes to figure out how many more methods there are to receive */
    if(recvf(client, buffer, 2) == -1) return -1;

    /* Check if first 2 bytes of the protocol are done properly*/
    if(buffer[METHODS_FIELD_VERSION] != SOCKS_VERSION || buffer[METHODS_FIELD_NMETHODS] == 0) return -1;

    /* Receive that amount of methods */
    if(recvf(client, buffer+2, buffer[1]) == -1) return -1;

    /* Variable for the method we choose, NOMETHOD by default */
    unsigned char method = METHODS_VAR_NOMETHOD;

    /* Loop over methods until we agree on one */
    for(index_t i = 2; i < buffer[METHODS_FIELD_NMETHODS]; ++i)
    {
        /* We prefer to proceed with no authentication */
        if(((unsigned char)buffer[i]) == METHODS_VAR_NOAUTH)
        {
            method = METHODS_VAR_NOAUTH;
            break;
        }
    }

    /* Send chosen method */
    char choice[2] = {SOCKS_VERSION, (char)method};
    sendf(client, choice, sizeof(choice));

    /* If we did not find any method we terminate */
    int ret_val = (method != METHODS_VAR_NOMETHOD) ? 0 : -1;
    return ret_val;

}


/* TODO: WHAT IF WE NEED TO DO A DNS LOOKUP */

/**
 *
 * @param client - client socket to get request from
 * @param ss - storage for ip and port from client's request
 * @return type of request
 */
static res_t evaluate_request(fd_t client, struct sockaddr_storage* ss)
{
    /* Buffer for client's request */
    char request[BUFSIZE];

    /* Receive first four bytes to determine the length of the rest */
    recvf(client, request, 4);

    /* Check if the 4 bytes of the protocol were done properly */
    if(request[0] != SOCKS_VERSION || request[2] != 0x00) return -1;

    switch(request[3])
    {
        case IPV4:
        {
            /* Get 4 byte IPv4-address and 2 byte port */
            recvf(client, request+4, 6);

            ss->ss_family = AF_INET;
            memcpy(getaddrss(ss), request + 4, 4); // IPv4 Address
            memcpy(getportss(ss), request + 8, 2); // Port

            break;
        }

        case IPV6:
        {
            /* Get 16 byte IPv6-address and 2 byte port */
            recvf(client, request+4, 18);

            ss->ss_family = AF_INET6;
            memcpy(getaddrss(ss), request + 4, 16); // IPv6 Address
            memcpy(getportss(ss), request + 20, 2); // Port

            break;
        }

        case DOMAINNAME:
        {
            /* Get length of the following domain name */
            recvf(client, request+4, 1);

            /* Receive domain name */
            recvf(client, request+5, request[4]);

            /* DNS resolution of the given doman */
            dnslookup(request+5, request[4]);


        }

        default:
            return -1;
    }

    return request[1];
}


/*
 * TODO: I dont like poll here, replace it with either epoll or libevent
 * The thing that worries me is bitwise & against signed integer
 * man poll doesnt say anything on it, cant find anymore info :/
 *
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
    fd_t client_sock = (fd_t)arg;

    if(exchange_methods(client_sock) == -1)
    {
        message(MSGTYPE_ERROR, "SOCKS5 - FAILED TO EXCHANGE METHODS");
        goto exit;
    }
    else
    {
        message(MSGTYPE_OK, "SOCKS5 - EXCHANGING METHODS WITH CLIENT");
    }

    /* struct to contain user's ip and port*/
    struct sockaddr_storage ss;

    /* We get user request and read target address and port to struct */
    res_t req_type = get_request(client_sock, &ss);
    if(req_type == -1)
    {
        message(MSGTYPE_ERROR, "INCORRECT REQUEST");
        goto exit;
    }

    switch(req_type)
    {
        case CONNECT:
        {
            fd_t server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(server_sock != 0)
            {
                message(MSGTYPE_ERROR, "COULDN'T CREATE A SOCKET");
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
        case BIND:
        {
            //r = socks5_bind_req(address, sizeof(address), port);

            break;
        }
        case UDP_ASSOCIATE:
        {
            //r = socks5_udpassoc_req(address, sizeof(address), port);

            break;
        }
        default:
            goto exit;
    }

    exit:
        shutdown(client_sock, 2);
        close(client_sock);
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
                FLAGS |= BM_OPT_IPV4;
                break;
            }
            case '6':
            {
                FLAGS |= BM_OPT_IPV6;
                break;
            }
            case 'd':
            {
                if(!(FLAGS & BM_OPT_VERBOSE))
                {
                    FLAGS |= BM_OPT_DAEMON;
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
                if(!(FLAGS & BM_OPT_DAEMON))
                {
                    FLAGS |= BM_OPT_VERBOSE;
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
                FLAGS |= BM_OPT_MAX_CLIENTS;
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
    if(!(FLAGS & BM_OPT_VERBOSE) && !(FLAGS & BM_OPT_DAEMON))
    {
        FLAGS |= BM_OPT_VERBOSE;
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --verbose");
    }

    if(!(FLAGS & BM_OPT_IPV4) && !(FLAGS & BM_OPT_IPV6))
    {
        FLAGS |= BM_OPT_IPV4;
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT OPTION --ipv4");
    }

    if(!(FLAGS & BM_OPT_MAX_CLIENTS))
    {
        message(MSGTYPE_WARNING, "ASSUMING DEFAULT ARGUMENT FOR OPTION --max-client=10 ");
    }




    /* Нам нужна getaddrinfo для бинда(так как сервер может быть ipv4 или 6, и проше будет если geatddrinfo
     * заполнит всё */

    struct addrinfo hints = {0};
    struct addrinfo *results, *p_res;


    hints.ai_family = (FLAGS & BM_OPT_IPV4) ? AF_INET : AF_INET6;
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
