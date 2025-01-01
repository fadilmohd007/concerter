#include <stdio.h>
#include <sqlite3.h>
#include <time.h>



#ifdef __cplusplus
extern "C"{
#endif 

#ifndef SQLITE_CONN_H
#define SQLITE_CONN_H                                      

int CreateDB(char* dbName, sqlite3 **db);
int InitAllTables(sqlite3 *db) ;
int DBExec(const char* dbQeury, sqlite3 *db);
int UpdateScore( char *name, sqlite3 *db, int new_score);


//Event
typedef struct {
    int eventId;
    char eventName[100];
    char eventStartDate[100];
    char eventEndDate[100];
    char eventLocation[100];
} event_t;

int SaveEvent(sqlite3 *db, const char *eventName, const char *eventDate, const char *eventEndDate, const char *eventLocation);
int GetEventById(sqlite3 *db, event_t *event,  int eventId);
int GetEventByName(sqlite3 *db, const char *eventName);

int SaveAttendee(sqlite3 *db, const char *name, const char *email, const char *mobileNum, int groupId);
int GetAttendeeById(sqlite3 *db, int attendeeId);
int GetAttendeeByName(sqlite3 *db, const char *name);

int DeleteEventById(sqlite3 *db, int eventId);
int DeleteAttendeeById(sqlite3 *db, int attendeeId);
int UpdateEvent(sqlite3 *db, int eventId, const char *eventName, const char *eventDate, const char *eventEndDate, const char *eventLocation);
int UpdateAttendee(sqlite3 *db, int attendeeId, const char *name, const char *email, const char *mobileNum, int groupId);

#endif //SQLITE_CONN_H

#ifdef __cplusplus
}
#endif

