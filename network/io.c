
#include "io.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>


bool send_all(fd_t s, const char* buf, size_t len)
{
    const char *p_buf = &buf[0];

    while(len > 0)
    {
        int b = send(s, p_buf, len, 0);
        if(b == -1) return FALSE;

        p_buf += b;
        len -= b;
    }

    return TRUE;
}

/* TODO: blocking recv allow to recv less than 1500 for example
 * Now its just blocking, like read till theres nothing to read and send it then
 * Wait fore more again and so on */
int recv_all(fd_t s, char* buf, size_t len)
{
    const size_t clen = len;
    char *p_buf = &buf[0];

    while(len > 0)
    {
        int b = recv(s, (void*)p_buf, len, 0);
        if(b == 0) return 0;
        if(b == len) return len;
        if(b == -1) return -1;

        p_buf += b;
        len -= b;
    }
    return clen;
}


void handle_data(fd_t client, fd_t server)
{
    struct pollfd fds[2];

    fds[0].fd = client;
    fds[0].events = POLLIN | POLLOUT;

    fds[1].fd = server;
    fds[1].events = POLLIN | POLLOUT;


    while(1)
    {
        int s = poll(fds, 2, 10000);
        if(s > 0)
        {
            if(fds[0].revents & POLLIN)
            {
                if(fds[1].revents & POLLOUT)
                {
                    char buffer[1500];
                    int b = recv_all(fds[0].fd, buffer, 1500);
                    if(b > 0) send_all(fds[1].fd, buffer, b);
                    else break;
                }
            }

            if(fds[1].revents & POLLIN)
            {
                if(fds[0].revents & POLLOUT)
                {
                    char buffer[1500];
                    int b = recv_all(fds[1].fd, buffer, 1500);
                    if(b > 0) send_all(fds[0].fd, buffer, b);
                    else break;
                }
            }
        }
        else
        {
            break;
        }
    }
}

