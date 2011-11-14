#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>

int GetSocket(const char* port);

void MainServerLoop()
{
    int sfd = GetSocket("2012");
    if (sfd == -1) return;

    sockaddr_storage peer_addr;
    int cfd;
    socklen_t peer_addr_size;

    while (true) {
        peer_addr_size = sizeof(sockaddr_storage);
        printf("Waiting for a connection!\n");
        cfd = accept(sfd, (sockaddr*) &peer_addr, &peer_addr_size);

        if (cfd == -1) {
            printf("Bad cfd\n");   
            continue;
        }

        
        printf("Hey a connection!\n");
        sleep(1);
        printf("Closing connection!\n");
        close(cfd);
    }
}
