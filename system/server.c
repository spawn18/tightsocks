#include "server.h"

#include "misc/defs.h"
#include "protocol/request.h"
#include "log.h"
#include "protocol/commands/connect.h"
#include "protocol/commands/bind.h"
#include "protocol/commands/udp_associate.h"
#include "protocol/method_exchange.h"
#include "system/options.h"
#include "firewall.h"
#include "protocol/reply.h"

#include <stdlib.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <misc/utils.h>


static int totalConns = 0;
static pthread_mutex_t mut;


void* handle_client(void* arg)
{
    sock_t client = *(sock_t*)arg;


    if(SOCKS_handle_method(client))
    {
        request_t req = {0};

        if(SOCKS_get_request(client, &req))
        {
            if(is_opt_set(OPT_LOG))
            {
                struct sockaddr_storage addr;
                socklen_t len = sizeof(addr);

                if(getpeername(client, (struct sockaddr*)&addr, &len) == 0)
                {
                    log_entry_t entry = {0};

                    addr_to_str(&addr, entry.srcHost, entry.srcPort);
                    cmd_to_str(req.CMD, entry.command);
                    atyp_to_str(req.ATYP, entry.addrType);
                    req_addr_to_str(req.ATYP, req.DSTADDR, req.DSTPORT, entry.dstHost, entry.dstPort);

                    log_write(&entry);
                }
            }

            if(is_opt_set(OPT_FIREWALL))
            {
                fw_rule_t rule;
                req_addr_to_str(req.ATYP, req.DSTADDR, req.DSTPORT, rule.host, rule.port);
                if(fw_find(&rule))
                {
                    SOCKS_reply_fail(client, REP_CONN_NOT_ALLOWED);
                    goto exit;
                }
            }

            if      (req.CMD == CMD_CONNECT)        SOCKS_connect(client, req.ATYP, req.DSTADDR, req.DSTPORT);
            else if (req.CMD == CMD_BIND)           SOCKS_bind(client, req.ATYP, req.DSTADDR, req.DSTPORT);
            else if (req.CMD == CMD_UDP_ASSOCIATE)  SOCKS_udp_associate(client, req.ATYP, req.DSTADDR, req.DSTPORT);
        }
    }

exit:;
    close(client);

    pthread_mutex_lock(&mut);
    --totalConns;
    pthread_mutex_unlock(&mut);

    free((int*)arg);
    pthread_exit(0);
}


void serve()
{
    sock_t s4;
    sock_t s6;

    if(is_opt_set(OPT_IP4))
    {
        s4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        int on = 1;
        setsockopt(s4, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(s4 == -1) exit(-1);

        struct sockaddr_in addr4;
        addr4.sin_family = AF_INET;
        addr4.sin_addr.s_addr = INADDR_ANY;
        addr4.sin_port = htons(PORT);

        if(bind(s4, (struct sockaddr*)&addr4, sizeof(addr4)) == -1) exit(-1);

        if(listen(s4, MAX_CONNS) == -1) exit(-1);


        fcntl(s4, F_SETFL, O_NONBLOCK);
    }


    if(is_opt_set(OPT_IP6))
    {
        s6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

        int on = 1;
        setsockopt(s6, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&on, sizeof(on));
        setsockopt(s6, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(s6 == -1) exit(-1);

        struct sockaddr_in6 addr6;
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(PORT);

        if(bind(s6, (struct sockaddr*)&addr6, sizeof(addr6)) == -1) exit(-1);

        if(listen(s6, MAX_CONNS) == -1) exit(-1);

        fcntl(s6, F_SETFL, O_NONBLOCK);
    }

    sock_t s = s4;
    option_t opt = OPT_IP4;


    while(1)
    {
        if (totalConns > MAX_CONNS) continue;

        if(is_opt_set(opt))
        {
            int client = accept(s, NULL, NULL);
            if(client != -1)
            {
                pthread_mutex_lock(&mut);
                ++totalConns;
                pthread_mutex_unlock(&mut);

                pthread_t t = 0;
                int* arg = malloc(sizeof(client));
                *arg = client;
                if (pthread_create(&t, NULL, handle_client, arg) == 0)
                {
                    pthread_detach(t);
                }
            }
        }

        opt = (opt == OPT_IP4) ? OPT_IP6  : OPT_IP4;
        s   = (s == s4)        ? s6       : s4;
    }
}