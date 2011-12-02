#ifndef _BUSLIST_H_
#define _BUSLIST_H_

#include <map>
#include <list>
#include "dbthread.h"

class BusList {
    std::map<int, std::list<busCoord>*> buslist;

    public:
    unsigned size();
    char* get_compressed_data(int route);
    void empty();
    void insert(int route, float x, float y);

};

#endif
