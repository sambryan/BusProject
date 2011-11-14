#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define LISTEN_BACKLOG 50

int GetSocket(const char* port)
{
    int sfd, s;
    addrinfo hints, *res, *rp;
    
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    s = getaddrinfo(NULL, port, &hints, &res);
    if (s) return -1;

    for ( rp = res; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;

        if ( bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0 )
            break;

        close(sfd);
    }

    if ( rp == NULL ) return -1;

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        return -1;

    freeaddrinfo(res);
    return sfd;
}
