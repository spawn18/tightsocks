#include "../include/io.h"
#include "../include/socks.h"
#include "../include/types.h"
#include "../include/convert.h"
#include "../include/message.h"

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

method_t exchange_methods(fd_t client)
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
    char choice[2] = {SOCKS_VERSION, method};
    sendf(client, choice, sizeof(choice));

    /* If we did not find any method we terminate */
    if (method != METHODS_VAR_NOMETHOD)
    {
        message(MSGTYPE_ERROR, "CLIENT REJECTED: NO METHOD CONSENSUS");
        close(client);
        pthread_exit(0);
    }

    return method;

}


void evaluate_request(fd_t client)
{
    /* Buffer for client's request */
    char request[BUFSIZE];

    /* Receive first four bytes to determine the length of the rest */
    recvf(client, request, 4);

    /* Check if the 4 bytes of the protocol were done properly */
    if(request[0] != SOCKS_VERSION || request[2] != 0x00) return -1;

    struct sockaddr_storage* ss;

    switch(request[3])
    {
        case ADDRTYPE_IPV4:
        {
            /* Get 4 byte IPv4-address and 2 byte port */
            recvf(client, request+4, 6);

            ss->ss_family = AF_INET;
            memcpy(getaddrss(ss), request + 4, 4); // IPv4 Address
            memcpy(getportss(ss), request + 8, 2); // Port

            break;
        }

        case ADDRTYPE_IPV6:
        {
            /* Get 16 byte IPv6-address and 2 byte port */
            recvf(client, request+4, 18);

            ss->ss_family = AF_INET6;
            memcpy(getaddrss(ss), request + 4, 16); // IPv6 Address
            memcpy(getportss(ss), request + 20, 2); // Port

            break;
        }

        case ADDRTYPE_DOMAINNAME:
        {
            /* Get length of the following domain name */
            recvf(client, request+4, 1);

            /* Receive domain name */
            recvf(client, request+5, request[4]);

            /* DNS resolution of the given doman */
            dnslookup(request+5, request[4]);

        }

        default:
            goto exit;
    }


    switch(request[1])
    {
        case REQTYPE_CONNECT:
        {
            fd_t server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(server_sock == -1)
            {
                if(FLAGS & OPT_VERBOSE)
                    message(MSGTYPE_ERROR, "COULDN'T CREATE A SOCKET");

                goto exit;
            }

            if(connect( server_sock, (struct sockaddr*)&ss, sizeof( *((struct sockaddr*)&ss) ) ) == -1)
            {
                if(FLAGS & OPT_VERBOSE)
                    message(MSGTYPE_ERROR, "COULDN'T ESTABLISH CONNECTION TO DESTINATION HOST");

            }

            errno_var = errno;

            break;
        }
        case REQTYPE_BIND:
        {

            break;
        }
        case REQTYPE_UDP_ASSOCIATE:
        {

            break;
        }
        default:
            goto exit;
    }

    return errno_var;

exit:
    shutdown(client, SHUT_RDWR);
    close(client);
    pthread_exit(0);

}
