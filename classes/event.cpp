#include <iostream>

#include "sqlite_conn.h"
#include "event.h"

void Event::printEvent() const
{
    cout << "Event ID: " << this->eventId << endl;
    cout << "Event Name: " << this->eventName << endl;
    cout << "Event Start Date: " << this->eventStartDate << endl;
    cout << "Event End Date: " << this->eventEndDate << endl;
    cout << "Event Location: " << this->eventLocation << endl;
}