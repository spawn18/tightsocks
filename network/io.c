
#include "io.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

/**
 * @brief send() wrapper that guarantees delivery (because send can fail sometimes)
 *
 * @param recvr Socket to write to
 * @param message Buffer to send
 * @param len Buffer size
 *
 * @return TRUE - success, FALSE - fail
 */
bool send_all(fd_t recvr, const char* msg, size_t len)
{
    const char *p_msg = &msg[0];
    size_t msgLen = len;

    while(msgLen > 0)
    {
        int bytesSent = send(recvr, p_msg, msgLen, 0);

        if(bytesSent == -1) return FALSE;

        p_msg += bytesSent;
        msgLen -= bytesSent;
    }

    return TRUE;
}


/**
 * Recv() wrapper that guarantees read (read() can read less bytes than advertised)
 *
 * @param sender Socket to read from
 * @param buf Buffer to write to
 * @param len Buffer size
 *
 * @return TRUE - success, FALSE - fail
 */
bool recv_all(fd_t sender, char* buf, size_t len)
{
    char *p_msg = &buf[0];
    size_t bufLen = len;

    while(bufLen > 0)
    {
        int bytesRecvd = recv(sender, (void*)p_msg, bufLen, 0);

        if(bytesRecvd == 0) return FALSE;
        if(bytesRecvd == -1) return -1;

        p_msg += bytesRecvd;
        bufLen -= bytesRecvd;
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

