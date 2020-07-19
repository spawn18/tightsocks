#include "../include/io.h"
#include "../include/socks.h"
#include "../include/types.h"
#include "../include/convert.h"
#include "../include/message.h"

#include <stdlib.h>
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


enum REPLY wait_for_request(fd_t client)
{

    /* Reply type of the destination host */
    enum REPLY reply = REPLY_GENERAL_ERROR;

    /* Address tyoe */
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
            reply = REPLY_ADDRESS_TYPE_NOT_SUPPORTED;
            goto exit;

    }

    /* Type of request */
    switch(request[1])
    {
        case REQTYPE_CONNECT:
        {
            fd_t server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(server_sock == -1)
            {
                if(FLAGS & OPT_VERBOSE)
                    message(MSGTYPE_ERROR, "COULDN'T CREATE A SOCKET");
                break;
            }

            if(connect( server_sock, (struct sockaddr*)&ss, sizeof( *((struct sockaddr*)&ss) ) ) == -1)
            {
                if(FLAGS & OPT_VERBOSE)
                    message(MSGTYPE_ERROR, "COULDN'T ESTABLISH CONNECTION TO DESTINATION HOST");

                switch(errno)
                {
                    case EPERM:
                        reply = REPLY_CONNECTION_NOT_ALLOWED;
                        break;
                    case EAFNOSUPPORT:
                        reply = REPLY_ADDRESS_TYPE_NOT_SUPPORTED;
                        break;
                    case ECONNREFUSED:
                    case ETIMEDOUT:
                        reply = REPLY_CONNECTION_REFUSED;
                        break;
                    case ENETUNREACH:
                        reply = REPLY_NETWORK_UNREACHABLE;
                        break;
                }
            }

            reply = REPLY_SUCCEEDED;

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
            reply = REPLY_COMMAND_NOT_SUPPORTED;
            break;
    }

    return
}

void socks_reply(fd_t client, enum REPLY reply, enum ADDRTYPE addrtype, const_address_t address, const_port_t port)
{
    char bytes = 0;
    switch(addrtype)
    {
        case ADDRTYPE_IPV4:
            bytes = 4;
            break;
        case ADDRTYPE_IPV6:
            bytes = 16;
            break;
        case ADDRTYPE_DOMAINNAME:
            bytes = 1 + address[0];
            break;
        default:
            // TODO: ERROR
            break;
    }

    char* rep = malloc(6 + bytes);

    rep[0] = SOCKS_VERSION;
    rep[1] = reply;
    rep[2] = 0x0;
    rep[3] = addrtype;
    memcpy(rep+4, address, bytes);
    memcpy(rep+4+bytes, port, 2);

    sendf(client, rep, 6+bytes);

}

inline index_t socks_get_port_index(const char* reply)
{
    index_t i = 0;
    switch(reply[4])
    {
        case ADDRTYPE_IPV4:
            i = 4;
            break;
        case ADDRTYPE_IPV6:
            i = 16;
            break;
        case ADDRTYPE_DOMAINNAME:
            i = reply[5] + 1;
            break;
    }

    return i+4;
}

inline index_t socks_get_address_size(const char* reply)
{
    size_t i = 0;
    switch(reply[4])
    {
        case ADDRTYPE_IPV4:
            i = 4;
            break;
        case ADDRTYPE_IPV6:
            i = 16;
            break;
        case ADDRTYPE_DOMAINNAME:
            i = reply[5];
            break;
    }

    return i;
}

void socks_set_port(char* reply,  port)
{

}

void request_connect(ADDRTYPE address_type, address_t address, port_t port)
{

}

void request_bind(ADDRTYPE address_type, address_t address, port_t port)
{

}

void request_associate(ADDRTYPE address_type, address_t address, port_t port)
{

}

void socks_get_request(fd_t client, socks_request_t* sr)
{
    /* Buffer for client's request */
    char request[BUFSIZE];

    /* Receive first 5 bytes to determine the length of the rest */
    recvf(client, request, 5);

    /* Error handling */
    if(request[0] != SOCKS_VERSION || request[2] != 0x00 || (request[1] < 0 || request[1] > 3)) {/* ERROR */}

    /* Read address and port accordingly */
    if (request[3] == ADDRESS_TYPE_DOMAINNAME)
    {
        recvf(client, request+5, request[4]+2);

        sr->address_size = request[4];
        sr->address = malloc(sr->address_size);
        memcpy(sr->address, (const void *)request[5], sr->address_size);
        sr->port = (((short)(request[4]+sr->address_size)) << 8) | (request[4]+sr->address_size+1);
    }
    else if (request[3] == ADDRESS_TYPE_IPV4)
    {
        recvf(client, request+5, 3);

        sr->address_size = 4;
        sr->address = malloc(sr->address_size);
        memcpy(sr->address, (const void *)request[4], sr->address_size);
        sr->port = (((short)(request[3]+4)) << 8) | (request[3]+5);
    }
    else if (request[3] == ADDRESS_TYPE_IPV6)
    {
        recvf(client, request+5, 15);

        sr->address_size = 16;
        sr->address = malloc(sr->address_size);
        memcpy(sr->address, (const void *)request[4], sr->address_size);
        sr->port = (((short)(request[3]+16)) << 8) | (request[3]+17);
    }
    else
    {
        /* ERROR */
    }

    /* Everything is read and now we fill up struct*/
    sr->version = SOCKS_VERSION;
    sr->command = request[1];
    sr->address_type = request[3];
}