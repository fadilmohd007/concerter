#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;
class Event 
{
    public:
        Event() : eventId(0), eventName(""), eventStartDate(0), eventEndDate(0), eventLocation("") {}
    private:
    int eventId;
    string eventName; 
    time_t eventStartDate;
    time_t eventEndDate;
    string eventLocation;
    public:
    void getEventById(int eventId, sqlite3 *db);
    void printEvent();
    // Event getEventbyName(string eventname);
};
