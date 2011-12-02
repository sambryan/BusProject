#ifndef _DBTHREAD_H_
#define _DBTHREAD_H_

#include <queue>
#include <semaphore.h>
#include "clientthread.h"
#include <vector>

struct Request {
    short route;
    short stop;
    int sfd;
};

struct busCoord {
    float x;
    float y;
};

struct DBData {
    std::queue<Request*>* reqQ;
    sem_t* semaQNotEmpty;
    sem_t* semaQNotFull;
    pthread_mutex_t* mreqQ;
    std::list<ClientInfo*>* clients;
    pthread_mutex_t* mclients;
};

typedef std::list<ClientInfo*>::iterator pClientListIter;

void* dbthread(void* data);
void SetBusLocations(std::vector<busCoord>** busloc);

#endif
