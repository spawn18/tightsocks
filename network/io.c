#include "io.h"
#include "system/options.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>

bool send_all(socket_t s, const char* buf, size_t len)
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

int recv_all(socket_t s, char* buf, size_t len)
{
    const size_t clen = len;
    char *p_buf = &buf[0];

    while(len > 0)
    {
        int b = recv(s, (void*)p_buf, len, 0);
        if(b == 0) return 0;
        if(b == -1) return -1;

        p_buf += b;
        len -= b;
    }
    return clen;
}

void handle_io(socket_t s1, socket_t s2)
{
    struct pollfd fds[2];

    fds[0].fd = s1;
    fds[0].events = POLLIN | POLLOUT;

    fds[1].fd = s2;
    fds[1].events = POLLIN | POLLOUT;

    char buf[8192];

    while(1)
    {
        int s = poll(fds, 2, 10000);
        if(s > 0)
        {
            if(fds[0].revents & POLLIN)
            {
                if(fds[1].revents & POLLOUT)
                {
                    int b = recv(fds[0].fd, buf, 8192, 0);
                    if(b > 0) send_all(fds[1].fd, buf, b);
                    else break;
                }
            }

            if(fds[1].revents & POLLIN)
            {
                if(fds[0].revents & POLLOUT)
                {
                    int b = recv(fds[1].fd, buf, 8192, 0);
                    if(b > 0) send_all(fds[0].fd, buf, b);
                    else break;
                }
            }
        }
        else break;
    }
}
