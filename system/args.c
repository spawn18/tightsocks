#include "args.h"

#include "options.h"
#include "system/log.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>


struct option long_options[] = {
        {"ipv4", 0, NULL, '4'},
        {"ipv6", 0, NULL, '6'},
        {"verbose", 0, NULL, 'v'},
        {"log", 0, NULL, 'l'},
        {"port", 1, NULL, 'p'},
        {"bufsize", 1, NULL, 'b'},
        {"auth", 1, NULL, 'a'},
        {"max-connections", 1, NULL, 'c'},
        {"help", 0, NULL, 'h'},
};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);

    int optc = 0;
    do
    {
        optc = getopt_long(argc, argv, "46vlp:b:a:c:h", long_options, NULL);

        switch(optc)
        {
            case '4':
            {
                SET_OPT(OPT_IP4);
                break;
            }

            case '6':
            {
                SET_OPT(OPT_IP6);
                break;
            }

            case 'v':
            {
                SET_OPT(OPT_VERBOSE);
                break;
            }

            case 'l':
            {
                SET_OPT(OPT_LOG);
                break;
            }

            case 'p':
            {
                SET_OPT(OPT_PORT);

                PORT = (int)strtol(optarg, NULL, 10);

                if(PORT < 0 || 65535 < PORT || errno == ERANGE)
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }

            case 'b':
            {
                SET_OPT(OPT_BUFSIZE);

                BUFSIZE = (int)strtol(optarg, NULL, 10);

                if(errno == ERANGE || BUFSIZE <= 0)
                {
                    usage(name);
                    exit(-1);
                }

                break;
            }

            case 'a':
            {
                SET_OPT(OPT_AUTH);

                char* user = strtok(optarg, " ");
                char* pass = strtok(NULL, " ");

                if(user == NULL || pass == NULL || strlen(user) > 255 || strlen(pass) > 255 || strtok(NULL, " ") != NULL)
                {
                    usage(name);
                    exit(-1);
                }
 
                USERNAME_LEN = strlen(user);
                PASSWORD_LEN = strlen(pass);

                USERNAME = malloc(sizeof(char) * USERNAME_LEN + 1);
                PASSWORD = malloc(sizeof(char) * PASSWORD_LEN + 1);

                strcpy(USERNAME, user);
                strcpy(PASSWORD, pass);

                printf("%s\n", USERNAME);
                printf("%s\n", PASSWORD);

                break;
            }

            case 'c':
            {
                SET_OPT(OPT_MAX_CONNS);

                int t = (int)strtol(optarg, NULL, 10);
                if(errno == ERANGE)
                {
                    usage(name);
                    exit(-1);
                }

                MAX_CONNS = t;
                break;
            }

            case 'h':
            {
                usage(name);
                exit(0);
            }

            case '?':
            {
                usage(name);
                exit(-1);
            }
        }
    }
    while(optc != -1);


    if(optind < argc)
    {
        usage(name);
        exit(-1);
    }
    
    if(!IS_OPT_SET(OPT_IP4) && !IS_OPT_SET(OPT_IP6))
    {
        SET_OPT(OPT_IP4);
        SET_OPT(OPT_IP6);
    }
}


void usage(char* name)
{
    printf("Usage: %s [OPTION]... \n\n"
           "  -4, --ipv4                                accept IPv4 connections\n"
           "  -6, --ipv6                                accept IPv6 connections\n"
           "                                            if unset, both are assumed\n"
           "  -l, --log                                 enable access logging to csv file\n"
           "  -p, --port=NUMBER                         set server port [default: 1080]\n"
           "  -c, --max-connections=LIMIT               limit for connections [default: 1024]\n"
           "  -b, --bufsize=SIZE                        set data buffer size in bytes [default: 65535]\n"
           "  -a, --auth=CREDENTIALS                    enable authentication with login and password\n"
           "                                            CREDENTIALS must be in the \"USERNAME PASSWORD\" form\n"
           "  -v, --verbose                             print information\n"
           "  -h, --help                                print this usage guide \n", name);
}

