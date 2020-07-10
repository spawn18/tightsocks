#include "../include/methods.h"
#include "../include/io.h"
#include "../include/message.h"

#include <unistd.h>
#include <pthread.h>

res_t method_userpass(fd_t client)
{
    /* Buffer alloc */
    char buffer[3];

    /* Get VER field and username length */
    recvf(client, buffer, 2);

    /* If protocol is okay */
    if(buffer[0] != 0x01)
    {
        close(client);
        message(MSGTYPE_WARNING, "INCORRECT SOCKS PROTOCOL DETECTED");
        pthread_exit(0);
    }

    /* Get username */
    char username[BUFSIZE/2];
    recvf(client, username, buffer[1]);

    /* Get length of password */
    recvf(client, buffer+2, 1);

    /* Get password */
    char password[BUFSIZE/2];
    recvf(client, password, buffer+2);

    /* Veridy Username/Password via database and proceed */






}