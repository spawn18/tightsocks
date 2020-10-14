#include "reply.h"
#include "request.h"
#include "network/io.h"

#include <string.h>

#define REP_LEN 261


bool socks_reply(fd_t client,
                const char rtyp,
                const char atyp,
                const char* host,
                const char* port)
{
    char rep[REP_LEN + 1];
    rep[4] = '\0';

    rep[0] = SOCKS_VER;
    rep[1] = rtyp;
    rep[2] = 0;
    rep[3] = atyp;
    strcat(rep, host);
    strcat(rep, port);

    return send_all(client, rep, strlen(rep));
}