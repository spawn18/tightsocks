
#include "io.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>


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


bool is_closed(fd_t s)
{

}

bool recv_all(fd_t s, char* buf, size_t len)
{
    char *p_buf = &buf[0];

    while(len > 0)
    {
        int b = recv(s, (void*)p_buf, len, 0);
        if(b == 0) return FALSE;
        if(b == -1) return FALSE;

        p_buf += b;
        len -= b;
    }
    return TRUE;
}


void stream(fd_t client, fd_t server)
{
    struct pollfd fds[2];

    fds[0].fd = client;
    fds[0].events = POLLIN | POLLOUT;

    fds[1].fd = server;
    fds[1].events = POLLIN | POLLOUT;


    while(1)
    {
        int pollStatus = poll(fds, 2, 3000);
        if(pollStatus > 0)
        {
            /* TODO: While not POLLHUP or something */
            if(fds[0].revents & POLLIN)
            {
                if(fds[1].revents & POLLOUT)
                {
                    char buffer[1500];
                    int bytesRecvd = recv_all(fds[0].fd, buffer, 1500);
                    if(bytesRecvd > 0)
                    {
                         send_all(fds[1].fd, buffer, bytesRecvd);
                    }
                }
            }

            if(fds[1].revents & POLLIN)
            {
                if(fds[0].revents & POLLOUT)
                {
                    char buffer[1500];
                    int bytesRecvd = recv_all(fds[1].fd, buffer, 1500);
                    if(bytesRecvd > 0)
                    {
                        send_all(fds[0].fd, buffer, bytesRecvd);
                    }
                }
            }
        }
        else
        {
            break;
        }
    }
}

