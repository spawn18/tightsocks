
#include "args.h"

#include "protocol/method_exchange.h"
#include "system/log.h"
#include "options.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#define MAX_CONNECTIONS_LIMIT 1024




struct option long_options[] = {
        {"ipv4", 0, 0, '4'},
        {"ipv6", 0, 0, '6'},
        {"daemon", 0, 0, 'd'},
        {"log", 0, 0, 'l'},
        {"help", 0, 0, 'h'},
        {"max-connections", 1, 0, 'c'},
        {"method", 1, 0, 'm'}
};

void handle_args(int argc, char** argv)
{
    char programName[strlen(argv[0]) + 1];
    strcpy(programName, argv[0]);

    // Loop over options
    int opt_char = 0;
    do
    {
        opt_char = getopt_long(argc, argv, "46dlhc:m:", long_options, NULL);

        switch(opt_char)
        {
            case 'h':
            {
                usage(programName);
                exit(0);
            }
            case '4':
            {
                set_opt(OPT_IPV4);
                break;
            }
            case '6':
            {
                set_opt(OPT_IPV6);
                break;
            }
            case 'd':
            {
                set_opt(OPT_DAEMON);
                break;
            }
            case 'l':
            {
                set_opt(OPT_LOG);
                break;
            }
            case 'c':
            {
                set_opt(OPT_MAX_CONNECTIONS);

                MAX_CONNECTIONS = (int)strtol(optarg, NULL, 10);
                if(MAX_CONNECTIONS == 0L || errno == ERANGE)
                {
                    fprintf(stderr, "Error: value for -c/--max-connections cannot be higher than 1024.");
                    exit(-1);
                }

                break;
            }
            case 'm':
            {
                set_opt(OPT_METHOD);

                if(strcmp(optarg, "userpass") == 0)
                {
                    PREFER_METHOD = METHOD_USERPASS;
                    break;
                }

                fprintf(stderr, "Error: incorrect value for -m/--method");
                exit(-1);
            }
            case '?':
            {
                exit(-1);
            }
        }
    }
    while(opt_char != -1);

    // Set default options
    if(!is_opt_set(OPT_IPV4) && !is_opt_set(OPT_IPV6))
        set_opt(OPT_IPV4);

    // Print messages if not daemon
    if(!is_opt_set(OPT_DAEMON))
    {
       if(is_opt_set(OPT_IPV4))
       {
           printf("IPv4 is enabled");
       }
       else if(is_opt_set(OPT_IPV6))
       {
           printf("IPv6 is enabled");
       }
       else if(is_opt_set(OPT_LOG))
       {
           printf("Logging is enabled");
       }
       else if(is_opt_set(OPT_MAX_CONNECTIONS))
       {
           printf("Max connections limit is set to %i", MAX_CONNECTIONS);
       }
       else if(is_opt_set(OPT_METHOD))
       {
           if(PREFER_METHOD == METHOD_USERPASS)
           {
               printf("SOCKS method set to username/password");
           }
       }
    }
}


void usage(char* progName)
{
    printf("Usage: %s [ -4 | -6 {-v | -d} -u (-c=K) ]  \n"
           "Usage: %s -h \n\n"
           "-4 --ipv4               Accept ipv4 clients [default]\n"
           "-6 --ipv6               Accept ipv6 clients (not recommended, see below)\n"
           "-d --daemon             Run as daemon (background process) \n"
           "-l --log                Make error logs in auto generated files\n"
           "-h --help               Print this usage guide - don't launch the program \n"
           "-c --max-connections=K  K is the maximum amount of connections that the \nserver is going to handle at a single moment of time  [default: 10]\n"
           "-m --method=M           Prefer method M when possible. \n List of valid names is given below\n"
           "Valid method names:\n* noauth - no authentication\n* userpass - username and password authentication\n"
           "\n If you're using \"userpass\" method you'll need to have a valid user first"
           "\n To add one, append the following line to \"users.txt\" file in \"users\" folder like so\n\n"
           "{username:admin,password:admin}\n\n"
           "To add multiple users just do the same on the newline. New user - new line", progName, progName);
}

