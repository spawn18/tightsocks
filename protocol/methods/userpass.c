
#include "network/io.h"
#include "system/log.h"
#include "system/options.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERS_RECORD_SIZE 528

static FILE* usersFile = NULL;

typedef struct user_
{
    char* username;
    char* password;
} user;

/**
 * @brief Handle username/password authentication
 * @note Hardcoded to use as less recv system calls as possible
 *
 * @param client Socket to get message from
 *
 * @return TRUE if succeed, FALSE otherwise
 */
bool handle_method_userpass(fd_t client)
{
    /* Version and username length*/
    char ver_and_usrname_len[2];
    recv_all(client, ver_and_usrname_len, 2);

    if(ver_and_usrname_len[0] == SOCKS_VERSION)
    {
        /* Get username */
        char usrname_and_pass_len[2];
        recvf(client, usrname_and_pass_len, 2);

        /* Get password */
        char password[usrname_and_pass_len[1]];
        recvf(client, password, usrname_and_pass_len[1]);

        /* Verify Username/Password via database and proceed */
        if(usersFile == NULL)
        {
            FILE* users = fopen("users.txt", "r");
        }

        user u;
        char* username = &usrname_and_pass_len[0];
        int userCounter = 1;
        bool userFound = FALSE;

        do
        {
            get_next_user(usersFile, u);
            userFound = strcmp(username, u.username) != 0 && strcmp(password, u.password) != 0
        }
        while(!userFound);

        if(userFound)
        {
            return TRUE;
        }
        else
        {

        }
    }
    else
    {
        LOG_MSG("INCORRECT PROTOCOL IN METHOD AUTHENTICATION");
    }

    return FALSE;
}


bool get_next_user(FILE *users, user u)
{
    char* entry;
    size_t size;

    getline(&entry, &size, users);

    char cleanEntry[size];
    remove_char(entry, cleanEntry, ' ');

    free(entry);

    // Correct size
    if(size <= MAX_USERS_RECORD_SIZE + 2)
    {
        // Correct beginning and end
        if(cleanEntry[0] == '{' && cleanEntry[size-1] == '}')
        {
            // Username check
            char usernameLen = 0;
            if(strncmp(&cleanEntry[1], "username:", 9) == 0)
            {
                char* username_p = &cleanEntry[10];

                for(int i = 0; i < 256; i++)
                {
                    usernameLen++;
                    if(*username_p++ == ',') break;
                }

                // Username correct
                if(*username_p == ',')
                {
                    strncpy(u.username, username_p, usernameLen);

                    // Password check
                    if(strncmp(&cleanEntry[10+usernameLen], "password:", 9) == 0)
                    {
                        char* password_p = &cleanEntry[20+usernameLen];
                        char  passwordLen = 0;

                        for(int i = 0; i < 256; i++)
                        {
                            if(*password_p++ == '}') break;
                        }

                        // Password correct
                        if(*password_p == '}')
                        {
                            strncpy(u.password, password_p, passwordLen);

                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

