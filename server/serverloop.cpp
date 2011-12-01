#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <cstdlib>
#include "clientthread.h"
#include "commandthread.h"
#include "dbthread.h"

#define LISTEN_BACKLOG 50
#define DEFAULT_SOCKET "2012"

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

void MainServerLoop(const char* port)
{
    /* init code */
    if ( !port )
        port = DEFAULT_SOCKET;

    int sfd = GetSocket(port);
    if (sfd == -1) return;

    sockaddr_storage peer_addr;
    int cfd;
    socklen_t peer_addr_size;
    pthread_t thread;
    pthread_attr_t thread_attr;

    pthread_mutex_t* mclients = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    *mclients = PTHREAD_MUTEX_INITIALIZER;
    std::list<ClientInfo*>* clients = new std::list<ClientInfo*>;

    /* launch dbthread */
    std::queue<Request*>* requestQueue = new std::queue<Request*>;
    sem_t* semaQ = (sem_t*)malloc(sizeof(sem_t) << 1);
    sem_init(semaQ, 0, 0);
    sem_init(semaQ + 1, 0, 1024);
    pthread_mutex_t* mreqQ = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    *mreqQ = PTHREAD_MUTEX_INITIALIZER;

    DBData* dbdata = (DBData*)malloc(sizeof(DBData));
    dbdata->reqQ = requestQueue;
    dbdata->semaQNotEmpty = semaQ;
    dbdata->semaQNotFull = semaQ + 1;
    dbdata->mreqQ = mreqQ;
    dbdata->clients = clients;
    dbdata->mclients = mclients;

    pthread_attr_init(&thread_attr);
    int ret = pthread_create(&thread, &thread_attr, &dbthread, dbdata);
    pthread_attr_destroy(&thread_attr);

    /* launch command line */
    Structs* structs = (Structs*)malloc(sizeof(structs));
    structs->clientList = clients;
    structs->mclientList = mclients;

    pthread_attr_init(&thread_attr);
    ret = pthread_create(&thread, &thread_attr, &CommandLine, structs);
    pthread_attr_destroy(&thread_attr);

    /* main loop */
    while (true) {
        peer_addr_size = sizeof(sockaddr_storage);
        cfd = accept(sfd, (sockaddr*) &peer_addr, &peer_addr_size);

        if (cfd == -1) {
            continue;
        }

        
        ClientThrdData* newThrdData = (ClientThrdData*)malloc(sizeof(ClientThrdData));
        newThrdData->sfd = cfd;
        newThrdData->clientList = clients;
        newThrdData->mclientList = mclients;

        pthread_attr_init(&thread_attr);
        ret = pthread_create(&thread, &thread_attr, &ClientThread, newThrdData);
        pthread_attr_destroy(&thread_attr);

    }
}
