#include "dbthread.h"
#include <time.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <sqlite3.h>

/* TO-DO:
 * CHECK QUEUE ON SEMA RELEASE
 * GET XML DATA
 * PARSE 
 * SEND FAKE TIME
 * BUS FULL */

void* dbthread(void* data)
{
    DBData* thrd = (DBData*)data;

    /* init db */
    sqlite3 *db;
    int rc = sqlite3_open("bus.db", &db);

    if (rc) {
        printf("Error: DB Failed to open: %s\n", sqlite3_errmsg(db));
        free(data);
        return NULL;
    }

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table';", -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if ( rc != SQLITE_ROW ) {
            // create table
            printf("Table does not exist. Creating... (RC: %d)\n", rc);
            rc = sqlite3_prepare_v2(db,
                    "CREATE TABLE bus_stops ( route INT, stop,  time INT, PRIMARY KEY ( route, stop, time ) );",
                    -1, &stmt, 0);
            if (rc) {
                printf("Failed to create table... %d\n", rc);
                sqlite3_close(db);
                free(data);
                return 0;
            }

            rc = sqlite3_step(stmt);

            if (rc != 101) {
                printf("Failed to create table... %d\n", rc);
                sqlite3_close(db);
                free(data);
                return 0;
            }

            sqlite3_finalize(stmt);

            rc = sqlite3_prepare_v2(db,
                    "INSERT INTO bus_stops VALUES ( 44, 01, 101 );",
                    -1, &stmt, 0);

            if (rc) {
                printf("Failed to prepare insert... %d\n", rc);
                sqlite3_close(db);
                free(data);
                return 0;
            }
            rc = sqlite3_step(stmt);

            if (rc != 101) {
                printf("Failed to execute insert... %d\n", rc);
                sqlite3_close(db);
                free(data);
                return 0;
            }
            sqlite3_finalize(stmt);
        }
        else {
            printf("Loading existing database...\n");
        }
    }
    else {
        printf("SQLite3 Error %d.\n", rc);
    }

    sqlite3_close(db);

    /* init vars */
    time_t lastUpdate = time(NULL);
    timespec timeout;
    timeout.tv_sec = lastUpdate + 30;
    timeout.tv_nsec = 0;
    std::vector<busCoord>* busLocations;

    /* Initial Fetch */

    SetBusLocations(&busLocations);

    /* Loop */
    while (true) {

        int err = sem_timedwait(thrd->semaQNotEmpty, &timeout);
        if (err == -1) {
            if (errno == ETIMEDOUT) {
                printf("DB: Fetching data...\n");

                // Fetch new list
                SetBusLocations(&busLocations);

                // Push to Clients
                pthread_mutex_lock(thrd->mclients);
                for (pClientListIter it = thrd->clients->begin(); it != thrd->clients->end(); it++) {
                    if ( (**it).route ) {
                        // Push GPS updates
                        printf("Pushing route %d\n", (**it).route);
                    }
                }
                pthread_mutex_unlock(thrd->mclients);

                lastUpdate = time(NULL);
            }
            else
                printf("Error dbthread on sem_timedwait (errno %d)\n", errno);
        }
        else {
            printf("DB: Processing Request...\n");
            // Process Request
            
            pthread_mutex_lock(thrd->mreqQ);
                Request* req = thrd->reqQ->front();
                thrd->reqQ->pop();
            pthread_mutex_unlock(thrd->mreqQ);

            // release sema

            // write (sfd) unix time to next bus
        }

        timeout.tv_sec = lastUpdate + 30;
        timeout.tv_nsec = 0;
    }

    /* close db */

    /* cleanup */
    free(data);
}

