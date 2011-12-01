#ifndef _COMMANDTHREAD_H_
#define _COMMANDTHREAD_H_

#include "clientthread.h"

struct Structs {
    std::list<ClientInfo*>* clientList;
    pthread_mutex_t* mclientList;
};

void* CommandLine(void* data);

#endif
