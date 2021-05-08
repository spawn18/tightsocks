#include "io.h"
#include "system/options.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>


bool send_all(sock_t s, const char* buf, size_t len)
{
    const char *ptr = &buf[0];

    while(len > 0)
    {
        int b = send(s, ptr, len, MSG_NOSIGNAL);
        if(b == -1) return FALSE;

        ptr += b;
        len -= b;
    }

    return TRUE;
}

int recv_all(sock_t s, char* buf, size_t len)
{
    const size_t clen = len;
    char *ptr = &buf[0];

    while(len > 0)
    {
        int b = recv(s, (void*)ptr, len, 0);
        if(b == 0) return 0;
        if(b == -1) return -1;

        ptr += b;
        len -= b;
    }
    return clen;
}

void handle_io(sock_t s1, sock_t s2)
{
    struct pollfd fds[2];

    fds[0].fd = s1;
    fds[0].events = POLLIN | POLLOUT;

    fds[1].fd = s2;
    fds[1].events = POLLIN | POLLOUT;

    char buf[BUFSIZE];

    while(1)
    {
        if(poll(fds, 2, 10000) <= 0) break;

        if(fds[0].revents & POLLIN)
        {
            if(fds[1].revents & POLLOUT)
            {
                int b = recv(fds[0].fd, buf, BUFSIZE, 0);
                if(b <= 0 || !send_all(fds[1].fd, buf, b)) break;
            }
        }

        if(fds[1].revents & POLLIN)
        {
            if((fds[0].revents & POLLOUT))
            {
                int b = recv(fds[1].fd, buf, BUFSIZE, 0);
                if(b <= 0 || !send_all(fds[0].fd, buf, b)) break;
            }
        }
    }
}
