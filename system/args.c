
#include "args.h"

#include "protocol/method_exchange.h"
#include "options.h"
#include "system/log.h"
#include "users.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>


struct option long_options[] ={
        {"ipv4", 0, NULL, '4'},
        {"ipv6", 0, NULL, '6'},
        {"log", 0, NULL, 'l'},
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

                if(!log_open()) exit(-1);

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
           "-4, --ipv4                          Accept IPv4 connections\n"
           "-6, --ipv6                          Accept IPv6 connections\n"
           "                                    If IP version is not set, both ipv4 and ipv6 are assumed\n"
           "-l, --log[=PATH]                    Enable logging to .csv file\n"
           "                                    If PATH is not set, then the file is created\n"
           "                                    in the same folder as executable\n"
           "-p, --port=NUMBER                   Set server port manually [default: 1080]\n"
           "-c, --max-connections=LIMIT         Limit for connections [default: 1024]\n"
           "-u, --user=\"USERNAME PASSWORD\"    Use username and password authentication for clients\n"
           "                                    USERNAME and PASSWORD must have\n"
           "                                    a length between 0 and 255\n"
           "                                    Multiple users are allowed\n"
           "-f, --firewall=\"COMMAND HOST\"     Deny requests with a rule set\n"
           "                                    COMMAND is CONNECT, BIND or UDP_ASSOCIATE\n"
           "                                    HOST can be a IPv4 or IPv6 address, FQDN or * for any host\n"
           "-h, --help                          Print this usage guide \n", name);
}

