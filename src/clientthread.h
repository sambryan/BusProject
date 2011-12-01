#ifndef _CLIENTTHREAD_H_
#define _CLIENTTHREAD_H_

#include <list>
#include <pthread.h>
#define BUF_SIZE 65536


struct ClientInfo {
    int sfd;
    int route;
};

struct ClientThrdData {
    int sfd;
    std::list<ClientInfo*>* clientList;
    pthread_mutex_t* mclientList;
};

typedef std::list<ClientInfo*>* pClientList;

void* ClientThread(void* data);

#endif
