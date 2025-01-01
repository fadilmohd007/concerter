#include <iostream>

#include "sqlite_conn.h"
#include "event.h"

using namespace std;
#include <ctime>

time_t convertStringToTimeT(const std::string &dateString)
{
    struct tm tm;
    if (strptime(dateString.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL)
    {
        cerr << "Error parsing date string" << endl;
        return -1;
    }
    return mktime(&tm);
}

void Event::getEventById(int eventId, sqlite3 *db)
{
    event_t event;
    int ret = 0;
    ret = GetEventById(db, &event, eventId);
    if (ret == SQLITE_OK)
    {
        this->eventId = event.eventId;
        this->eventName = std::string(event.eventName);
        this->eventStartDate = convertStringToTimeT(event.eventStartDate);
        this->eventEndDate = convertStringToTimeT(event.eventEndDate);
        this->eventLocation = std::string(event.eventLocation);
    }
    else
    {
        cerr << "Error retrieving event from database" << endl;
    }

    return;
}

void Event::printEvent()
{
    cout << "Event ID: " << this->eventId << endl;
    cout << "Event Name: " << this->eventName << endl;
    cout << "Event Start Date: " << this->eventStartDate << endl;
    cout << "Event End Date: " << this->eventEndDate << endl;
    cout << "Event Location: " << this->eventLocation << endl;
}