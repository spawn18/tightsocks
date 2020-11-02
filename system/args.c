
#include "args.h"

#include "protocol/method_exchange.h"
#include "options.h"
#include "system/log.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>


struct option long_options[] ={
        {"ipv4", 0, 0, '4'},
        {"ipv6", 0, 0, '6'},
        {"log", 0, 0, 'l'},
        {"port", 1, 0, 'p'},
        {"max-connections", 1, 0, 'c'},
        {"method", 1, 0, 'm'},
        {"firewall", 0, 0, 'f'},
        {"decline", 1, 0, 'd'},
        {"bufsize", 1, 0, 'b'},
        {"help", 0, 0, 'h'},

};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);


    int opt_char = 0;
    do
    {
        opt_char = getopt_long(argc, argv, "46lp:c:m:d:b:h", long_options, NULL);

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
            case 'm':
            {
                set_opt(OPT_METHOD);

                if(strcmp(optarg, "noauth") == 0)
                {
                    METHOD_PREFERED = METHOD_NOMETHOD;
                }
                else if(strcmp(optarg, "userpass") == 0)
                {
                    METHOD_PREFERED = METHOD_USERPASS;
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
            case 'b':
            {
                set_opt(OPT_BUFSIZE);

                int v = strtol(optarg, NULL, 10);
                if((v == 2 || v == 4 || v == 8 || v == 16  || v == 32 || v == 64) && errno != ERANGE)
                {
                    BUFSIZE = (int)pow(2, 10+log2(v));
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
           "-4, --ipv4                          Accept IPv4 connections\n"
           "-6, --ipv6                          Accept IPv6 connections\n"
           "                                    If IP version is not set, both ipv4 and ipv6 are assumed\n"
           "-l, --log[=PATH]                    Enable logging to .csv file\n"
           "                                    If PATH is not set, then the file is located\n"
           "                                    in the same folder as executable\n"
           "-p, --port=NUMBER                   Set server port manually [default: 1080]\n"
           "-c, --max-connections=LIMIT         Limit for connections [default: 1024]\n"
           "-m, --method=NAME                   Authentication method used for clients\n"
           "                                    Supported methods:\n"
           "                                    * noauth - no authentication [default]\n"
           "                                    * userpass - username and password\n"
           "                                      for userpass there needs to be at least one user"
           "                                      set with -u/--user option\n"
           "-u, --user={USERNAME PASSWORD}      Add user to authenticate in userpass method\n"
           "                                    Is only valid if method set to userpass\n"
           "                                    USERNAME and PASSWORD must be delimited by one whitespace\n"
           "                                    and have a length between 0 and 255 \n"
           "-f, --firewall=ADDRESS              Blacklist IP address used in requests\n"
           "                                    ADDRESS must be a valid IP address, and not a domain name\n"
           "-d, --decline=NUMBER                Decline requests that have a field set\n"
           "                                    To add multiple values, use option multiple times\n"
           "                                    * 1 - CMD is CONNECT\n"
           "                                    * 2 - CMD is BIND\n"
           "                                    * 3 - CMD is UDPASSOCIATE\n"
           "                                    * 4 - ATYP is IPV4\n"
           "                                    * 5 - ATYP is IPV6\n"
           "                                    * 6 - ATYP is DOMAINNAME\n"
           "-b, --bufsize=SIZE                  Set buffer size for TCP connections in kilobytes\n"
           "                                    2, 4, 8, 16, 32 or 64\n"
           "                                    For advanced users only [default: 8]\n"
           "-h, --help                          Print this usage guide \n", name);
}

