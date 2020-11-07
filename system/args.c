#include "args.h"

#include "protocol/method_exchange.h"
#include "options.h"
#include "system/log.h"
#include "firewall.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>


struct option long_options[] ={
        {"ipv4", 0, NULL, '4'},
        {"ipv6", 0, NULL, '6'},
        {"log", 2, NULL, 'l'},
        {"port", 1, NULL, 'p'},
        {"max-connections", 1, NULL, 'c'},
        {"user", 1, NULL, 'u'},
        {"firewall", 1, NULL, 'f'},
        {"help", 0, NULL, 'h'},
};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);

    int optc = 0;
    do
    {
        optc = getopt_long(argc, argv, "46lp:c:u:h", long_options, NULL);

        switch(optc)
        {
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
            case 'l':
            {
                set_opt(OPT_LOG);

                bool isOpen = FALSE;

                if(optarg)
                {
                    isOpen = log_open(optarg);
                }
                else
                {
                    isOpen = log_open(NULL);
                }

                if(!isOpen) exit(-1);

                break;
            }
            case 'p':
            {
                set_opt(OPT_PORT);

                long tmp = strtol(optarg, NULL, 10);
                if(0 < tmp && tmp < 65535 && errno != ERANGE)
                {
                    PORT = tmp;
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'c':
            {
                set_opt(OPT_MAX_CONNS);

                int t = (int)strtol(optarg, NULL, 10);
                if(errno == ERANGE)
                {
                    MAX_CONNS = t;
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'u':
            {
                set_opt(OPT_USERPASS);

                char* user = strtok(optarg, " \"");
                char* pass = strtok(NULL, " \n");

                if(user != NULL && pass != NULL && strlen(user) <= 255 && strlen(pass) <= 255)
                {
                    strcpy(USERNAME, user);
                    strcpy(PASSWORD, pass);
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'f':
            {
                set_opt(OPT_FIREWALL);

                char* host = strtok(optarg, " \"");
                char* port = strtok(NULL," \"");

                if(host != NULL && port != NULL)
                {
                    if(strlen(port) > 0 && strlen(port) < 6)
                    {
                        fw_rule_t rule = {0};
                        strcpy(rule.host, host);
                        strcpy(rule.port, port);
                        fw_add(&rule);
                    }
                }
                else
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }
            case 'h':
            {
                usage(name);
                exit(0);
            }
            case '?':
            {
                exit(-1);
            }
        }
    }
    while(optc != -1);

    if(!is_opt_set(OPT_IP4) && !is_opt_set(OPT_IP6))
    {
        set_opt(OPT_IP4);
        set_opt(OPT_IP6);
    }
}


void usage(char* name)
{
    printf("Usage: %s [OPTION]... \n\n"
           "  -4, --ipv4                                accept IPv4 connections\n"
           "  -6, --ipv6                                accept IPv6 connections\n"
           "                                            if unset, both are assumed\n"
           "  -l, --log[=PATH]                          enable logging to .csv file\n"
           "  -p, --port=NUMBER                         set server port [default: 1080]\n"
           "  -c, --max-connections=LIMIT               limit for connections [default: 1024]\n"
           "  -u, --user=\"USERNAME PASSWORD\"            authenticate with username and password\n"
           "                                            USERNAME and PASSWORD must have\n"
           "                                            a length between 0 and 255\n"
           "  -f, --firewall=\"HOST PORT\"                a simple firewall up to 100 entries\n"
           "                                            HOST must be an IPv4, IPv6 or FQDN\n"
           "                                            FQDN is not resolved. Only works if DNS is proxied\n"
           "  -h, --help                                print this usage guide \n", name);
}

