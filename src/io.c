
#include <stddef.h>
#include <sys/socket.h>

#include "../include/io.h"

/**
 * Send() wrapper that guarantees delivery (because send can fail sometimes)
 *
 * @param sock Socket to write to
 * @param buf Buffer to send
 * @param len Buffer size
 *
 * @return Same as send(): returns 0 on success -1 on error
 */
res_t sendf(fd_t sock, const void* buf, size_t len)
{
    char* p_buf = (char*) &buf;
    size_t buf_size = len;

    while(buf_size > 0)
    {
        int bytes_sent = send(sock, p_buf, buf_size, 0);

        if(bytes_sent == -1) return -1;

        p_buf += bytes_sent;
        buf_size -= bytes_sent;
    }
    return 0;
}

/**
 * Recv() wrapper that guarantees read (read can write less bytes than advertised)
 *
 * @param sock Socket to read from
 * @param buf Buffer to write to
 * @param len Buffer size
 *
 * @return Same as recv(): returns bytes read, -1 on error, 0 if connection is closed.
 */
res_t recvf(fd_t sock, char* buf, size_t len)
{
    char* p_buf = &buf[0];
    size_t buf_size = len;

    while(buf_size > 0)
    {
        int bytes_recv = recv(sock, p_buf, buf_size, 0);

        if(bytes_recv == 0) return 0;

        if(bytes_recv == -1) return -1;

        p_buf += bytes_recv;
        buf_size -= bytes_recv;

    }
    return len;
}