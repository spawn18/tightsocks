
#include "args.h"

#include "protocol/method_exchange.h"
#include "options.h"
#include "system/log.h"
#include "users.h"
#include "firewall.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>


struct option long_options[] ={
        {"ipv4", 0, NULL, '4'},
        {"ipv6", 0, NULL, '6'},
        {"log", 2, NULL, 'l'},
        {"port", 1, NULL, 'p'},
        {"max-connections", 1, NULL, 'c'},
        {"method", 1, NULL, 'm'},
        {"user", 1, NULL, 'u'},
        {"firewall", 0, NULL, 'f'},
        {"decline", 1, NULL, 'd'},
        {"bufsize", 1, NULL, 'b'},
        {"help", 0, NULL, 'h'},

};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);


    int opt_char = 0;
    do
    {
        opt_char = getopt_long(argc, argv, "46lp:c:u:f:h", long_options, NULL);

        switch(opt_char)
        {
            case '4':
            {
                set_opt(OPT_IP4);
                break;
            }
            case '6':
            {
                set_opt(OPT_IP6);
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
                set_opt(OPT_MAX_CONNECTIONS);

                int t = (int)strtol(optarg, NULL, 10);
                if(errno == ERANGE)
                {
                    MAX_CONNECTIONS = t;
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

                if(user != NULL && pass != NULL)
                {
                    users_init();

                    user_t u = {'\0'};
                    strcpy(u.username, user);
                    strcpy(u.password, pass);

                    users_add(&u);
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

                char* cmd = strtok(optarg, " ");
                char* host = strtok(NULL, " ");

                if(cmd != NULL && host != NULL && strlen(host) <= 255)
                {
                    fw_init();

                    fw_rule_t rule;

                    if(strcmp(cmd, "CONNECT") == 0) rule.cmd = CMD_CONNECT;
                    else if(strcmp(cmd, "BIND") == 0) rule.cmd = CMD_BIND;
                    else if(strcmp(cmd, "UDP_ASSOCIATE") == 0) rule.cmd = CMD_UDP_ASSOCIATE;
                    else
                    {
                        usage(name);
                        exit(-1);
                    }

                    strcpy(rule.host, host);

                    fw_add(&rule);
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
    while(opt_char != -1);

    if(!is_opt_set(OPT_IP4) && !is_opt_set(OPT_IP6))
    {
        set_opt(OPT_IP4);
        set_opt(OPT_IP6);
    }
}


void usage(char* name)
{
    printf("Usage: %s [OPTION]... \n\n"
           "  -4, --ipv4                          accept IPv4 connections\n"
           "  -6, --ipv6                          accept IPv6 connections\n"
           "                                      if unset, both are assumed\n"
           "  -l, --log[=PATH]                    enable logging to .csv file\n"
           "  -p, --port=NUMBER                   set server port [default: 1080]\n"
           "  -c, --max-connections=LIMIT         limit for connections [default: 1024]\n"
           "  -u, --user=\"USERNAME PASSWORD\"      use username and password authentication for clients\n"
           "                                      USERNAME and PASSWORD must have\n"
           "                                      a length between 0 and 255\n"
           "                                      multiple users are allowed\n"
           "  -f, --firewall=\"CMD HOST\"           deny requests with a rule set\n"
           "                                      CMD is CONNECT, BIND or UDP_ASSOCIATE\n"
           "                                      HOST can be a IPv4 or IPv6 address, FQDN or * for any host\n"
           "  -h, --help                          print this usage guide \n", name);
}

