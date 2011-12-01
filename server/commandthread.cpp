#include "commandthread.h"
#include <cstdio>
#include <iostream>
#include <string>

void* CommandLine(void* data)
{
    Structs* structs = (Structs*)data;

    std::string input;

    while (true) {
        printf(">> ");
        getline(std::cin, input);
        if (input.compare("status") == 0) {
            pthread_mutex_lock(structs->mclientList);
            printf("Number of connections: %d\n", structs->clientList->size());
            pthread_mutex_unlock(structs->mclientList);
        }
    }

    return NULL;
}

