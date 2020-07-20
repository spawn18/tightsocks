#include "../include/io.h"
#include "../include/socks.h"
#include "../include/types.h"
#include "../include/convert.h"
#include "../include/message.h"
#include "../include/utils.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>


method_t exchange_methods(fd_t client, method_t* method)
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
    unsigned char met = METHODS_VAR_NOMETHOD;

    /* Loop over methods until we agree on one */
    for(index_t i = 2; i < buffer[METHODS_FIELD_NMETHODS]; ++i)
    {
        /* We prefer to proceed with no authentication */
        if(((unsigned char)buffer[i]) == METHODS_VAR_NOAUTH)
        {
            met = METHODS_VAR_NOAUTH;
            break;
        }
    }

    /* Send chosen method */
    char choice[2] = {SOCKS_VERSION, met};
    sendf(client, choice, sizeof(choice));

    /* If we did not find any method we terminate */
    if (met != METHODS_VAR_NOMETHOD)
    {
        message(MSGTYPE_ERROR, "CLIENT REJECTED: NO METHOD CONSENSUS");
        close(client);
        pthread_exit(0);
    }

    *method = met;

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




void request_connect(socks_request_t request)
{
    fd_t server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == -1)
    {
        if(is_option_set(OPT_VERBOSE))
        {
            message(MSGTYPE_ERROR, "COULDN'T CREATE A SOCKET");
        }
    }

    struct sockaddr_storage ss;
    fill_ss(&ss, address_type, address_size, address, port);

    REPLY reply = REPLY_SUCCEEDED;
    if(connect(server, (struct sockaddr*)&ss, sizeof(*((struct sockaddr*)&ss))) == -1)
    {
        if(is_option_set(OPT_VERBOSE))
        {
            message(MSGTYPE_ERROR, "COULDN'T ESTABLISH CONNECTION TO DESTINATION HOST");
        }

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

    /* TODO: Кто должен выполнять реплай?
     * С одной стороны это просто реквест который выполняет сам реквест и не
     * должен думать о реплае
     * с другой стороны он один знает инфу о том как выполнился он и поэтому
     * может сразу дать ответ. Реплай зависит от типа реквеста. Возвращать реплай от выполнения
     * реквеста было бы глупо. Если что и стоило бы возвращать так это код ошибки
     * Наверное, реплай являясь частью реквеста и уникален для каждого типа реквеста
     * должен всё таки быть частью реквеста? */
}

void request_bind(ADDRESS_TYPE address_type, address_t address, port_t port)
{

}

void request_udp_associate(ADDRERSS_TYPE address_type, address_t address, port_t port)
{

}

void socks_get_request(fd_t client, socks_request_t request)
{
    /* Allocate space for first 5 bytes */
    request = malloc(sizeof(char)*5);
    if(request == NULL) { /* ERROR */ }

    /* Receive first 5 bytes to determine the length of the rest */
    recvf(client, request, 5);

    /* Error handling */
    if(request[0] != SOCKS_VERSION || request[2] != 0x00 || (request[1] < 0 || request[1] > 3)) {/* ERROR */}

    /* Read address and port accordingly */
    if (request[3] == ADDRESS_TYPE_DOMAINNAME)
    {
        request = realloc(request, 5+request[4]+2);
        recvf(client, request+5, request[4]+2);
    }
    else if (request[3] == ADDRESS_TYPE_IPV4)
    {
        request = realloc(request, 5+4+2);
        recvf(client, request+5, 3+2);
    }
    else if (request[3] == ADDRESS_TYPE_IPV6)
    {
        request = realloc(request, 5+15+2);
        recvf(client, request+5, 15+2);
    }
    else
    {
        /* ERROR */
    }

}