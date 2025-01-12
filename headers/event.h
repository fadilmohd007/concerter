#include <iostream>
#include <string>
#include <sqlite3.h>

#include "dateServices.h"
#ifndef EVENT_H
#define EVENT_H


using namespace std;
class Event
{
public:
    int getEventId() const { return eventId; }
    string getEventName() const { return eventName; }
    time_t getEventStartDate() const { return eventStartDate; }
    time_t getEventEndDate() const { return eventEndDate; }

    string getEventStartDateAsString() const { return convertTimeToString(eventStartDate); }
    string getEventEndDateAsString() const { return convertTimeToString(eventEndDate); }
    string getEventLocation() const { return eventLocation; }
    void setEventId(int id) { eventId = id; }
    void setEventName(const string &name) { eventName = name; }
    void setEventStartDate(const string &startDate) { eventStartDate = convertStringToTimeT(startDate); }
    void setEventEndDate(const string &endDate) { eventEndDate = convertStringToTimeT(endDate); }
    void setEventLocation(const string &location) { eventLocation = location; }
    Event() : eventId(0), eventName(""), eventStartDate(0), eventEndDate(0), eventLocation("") {}

private:
    int eventId;
    string eventName;
    time_t eventStartDate;
    time_t eventEndDate;
    string eventLocation;

public:
    void printEvent() const;
};

#endif // EVENT_H