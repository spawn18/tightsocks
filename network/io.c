#include "io.h"
#include "system/options.h"

#include <stddef.h>
#include <sys/socket.h>
#include <poll.h>

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

int recv_all(fd_t s, char* buf, size_t len)
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

