#include "buslist.h"
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>

unsigned BusList::size()
{
    return buslist.size();
}

char* BusList::get_compressed_data(int route)
{
    // work
    int retsize = 0;
    char* ret = NULL;

    if (buslist.count(route)) {
        for ( std::list<busCoord>::iterator it = buslist[route]->begin(); it != buslist[route]->end(); it++) {
            if (!ret) {
                ret = (char*)malloc(8);
                retsize = 8;
            }
            else {
                ret = (char*)realloc(ret, retsize + 8);
                retsize += 8;
            }

            short x = htons(it->x);
            short y = htons(it->y);

            memcpy(ret + retsize - 8, &x, 4);
            memcpy(ret + retsize - 4, &y, 4);
        }
    }
    

}

void BusList::empty()
{
    // iterate through list;
    std::map<int, std::list<busCoord>*>::iterator it = buslist.begin();
    for ( it; it != buslist.end(); it++) {
        it->second->clear();
        free(it->second);
    }

    buslist.clear();
}

void BusList::insert(int route, float x, float y)
{
    if (!buslist.count(route)) {
        buslist[route] = new std::list<busCoord>;
    }
    busCoord t;
    t.x = x;
    t.y = y;
    buslist[route]->push_back(t);
}

