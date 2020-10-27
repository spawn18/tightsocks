
#include "args.h"

#include "protocol/method_exchange.h"
#include "options.h"
#include "system/log.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>


struct option long_options[] = {
        {"ip4", 0, 0, '4'},
        {"ip6", 0, 0, '6'},
        {"port", 1, 0, 'p'},
        {"max-connections", 1, 0, 'c'},
        {"method", 1, 0, 'm'},
        {"help", 0, 0, 'h'},
        {"log", 0, 0, 'l'}
};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);


    int opt_char = 0;
    do
    {
        opt_char = getopt_long(argc, argv, "h46lp:c:m:", long_options, NULL);

        switch(opt_char)
        {
            case 'h':
            {
                usage(name);
                exit(0);
            }
            case '4':
            {
                set_opt(OPT_IP4);
                break;
            }
            case '6':
            {
                set_opt(OPT_IP6);
                break;
            }
            case 'd':
            {
                set_opt(OPT_DECLINE);

                if(*optarg == 1)
                {
                    set_decline(DECLINE_CONNECT);
                }
                else if(*optarg == 2)
                {
                    set_decline(DECLINE_BIND);
                }
                else if(*optarg == 3)
                {
                    set_decline(DECLINE_UDP_ASSOCIATE);
                }
                else if(*optarg == 4)
                {
                    set_decline(DECLINE_IPV4);
                }
                else if(*optarg == 5)
                {
                    set_decline(DECLINE_IPV6);
                }
                else if(*optarg == 6)
                {
                    set_decline(DECLINE_DOMAINNAME);
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'l':
            {
                set_opt(OPT_LOG);
                if(!log_open())
                    exit(-1);
                break;
            }
            case 'p':
            {
                set_opt(OPT_PORT);

                long tmp = strtol(optarg, NULL, 10);
                if(tmp < 0 || 65535 < tmp || errno == ERANGE)
                {
                    usage(name);
                    exit(-1);
                }

                PORT = (short)tmp;

                break;
            }
            case 'c':
            {
                set_opt(OPT_MAX_CONNECTIONS);

                MAX_CONNECTIONS = (int)strtol(optarg, NULL, 10);
                if(MAX_CONNECTIONS <= 0 ||  errno == ERANGE)
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'm':
            {
                set_opt(OPT_METHOD);

                if(*optarg == 0)
                {
                    METHOD_PREFERED = METHOD_NOMETHOD;
                    break;
                }
                else if(*optarg == 1)
                {
                    METHOD_PREFERED = METHOD_USERPASS;
                    break;
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case '?':
            {
                exit(-1);
            }
        }
    }
    while(opt_char != -1);

    if(!is_opt_set(OPT_IP4) && !is_opt_set(OPT_IP6))
        set_opt(OPT_IP4);
}


void usage(char* name)
{
    printf("Usage: %s [OPTION]... \n\n"
           "-h, --help                                              Print this usage guide \n"
           "-4, --ip4                                               Accept ipv4 clients [default]\n"
           "-6, --ip6                                               Accept ipv6 clients \n"
           "-d, --decline=FIELD                                     Decline requests that have field set\n"
           "                                                        To add multiple values, use option multiple times\n"
           "                                                        1 - CONNECT command\n"
           "                                                        2 - BIND command\n"
           "                                                        3 - UDP ASSOCIATE command\n"
           "                                                        4 - IPv4 address\n"
           "                                                        5 - IPv6 address\n"
           "                                                        6 - Domain name\n"
           "-l, --log                                               Enable logging to .csv file"
           "-p, --port=NUMBER                                Set server port manually. Range 1024-65535 [default: 1080]\n"
           "-c, --max-connections=LIMIT                       Connection limit for server  [default: 1024]\n"
           "-m, --method=NAME                                  Authentication method used for clients [default: noauth]\n"
           "                                                        Supported methods:\n"
           "                                                        * 0 - no authentication\n"
           "                                                        * 1 - username and password authentication\n", name);
}

