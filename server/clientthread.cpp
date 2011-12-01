#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <stdint.h>
#include <arpa/inet.h>
#include "clientthread.h"
#include <errno.h>

void SendACK(int sfd)
{
    uint16_t type = htons(1);
    if ( write(sfd, ((char*)&type)+1, 1) == -1 )
        printf("Error sending ACK: %d\n", errno);
    else
        printf("Sent ACK\n");
}

void SendNAK(int sfd)
{
    uint16_t type = htons(2);
    write(sfd, ((char*)type)+1, 1);
    if ( write(sfd, ((char*)&type)+1, 1) == -1 )
        printf("Error sending NAK: %d\n", errno);
    else
        printf("Sent NAK\n");
}

void* ClientThread(void* data)
{
    ClientThrdData* thrd = (ClientThrdData*)data;

    /* Init code */
    // printf("sfd %3d: Client Hello\n", thrd->sfd);
    char buffer[BUF_SIZE];

    ClientInfo* info = (ClientInfo*) malloc(sizeof(ClientInfo));
    info->sfd = thrd->sfd;
    info->route = 0;

    pthread_mutex_lock(thrd->mclientList);
    thrd->clientList->push_back(info);
    pthread_mutex_unlock(thrd->mclientList);
    
    /* client loop */
    while (true) {
        int ret = read(thrd->sfd, &buffer, BUF_SIZE);

        if (!ret) {
            close(thrd->sfd);
            break;
        }

        if (ret == -1) {
            break;
        }

        int bytesProcessed = 0;
        while ( bytesProcessed < ret ) {
            uint16_t type = buffer[bytesProcessed]; 
            
            switch (type) {
                case 0x1:
                    printf("Received type 1\n");
                    pthread_mutex_lock(thrd->mclientList);
                        info->route = buffer[++bytesProcessed];
                    pthread_mutex_unlock(thrd->mclientList);
                    printf("Route: %d\n", buffer[bytesProcessed]);
                    bytesProcessed++;
                    SendACK(thrd->sfd);
                    break;
                case 0x2:
                    printf("Received type 2\n");
                    printf("Route: %d\n", (buffer[++bytesProcessed]));
                    printf("Stop: %d\n", (buffer[++bytesProcessed]));
                    bytesProcessed++;
                    SendACK(thrd->sfd);
                    break;
                case 0x3:
                    printf("Received type 3\n");
                    printf("Route: %d\n", (buffer[++bytesProcessed]));
                    printf("Stop: %d\n", (buffer[++bytesProcessed]));
                    printf("Full: %d\n", (buffer[++bytesProcessed]));
                    bytesProcessed++;
                    SendACK(thrd->sfd);
                    break;
                case 0x4:
                    printf("Received type 4\n");
                    bytesProcessed++;
                    SendACK(thrd->sfd);
                    break;
                default:
                    printf("Received unrecognized type\n");
                    bytesProcessed++;
                    SendNAK(thrd->sfd);
                    break;
            }
            printf("\n");
        }
    }

    /* Cleanup Code */
    pthread_mutex_lock(thrd->mclientList);
    thrd->clientList->remove(info);
    pthread_mutex_unlock(thrd->mclientList);

    free(data);
}
