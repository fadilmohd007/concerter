#include <iostream>
#include "attendee.h"
#include "event.h"

#ifndef TICKET_H
#define TICKET_H

class Ticket 
{
    public:
        int VerifyTicket();
        int ticketId;
        //Number of entries made using this ticket
        int entryCount {0};
        int entriesLeft;
        int isVIP;
        //Foreign key of attendee
        int  attendeeId;
        //Foreign key of event
        int eventId;

        int getTicketId() const {
            return ticketId;
        }

        void setTicketId(int id) {
            ticketId = id;
        }

        int getEventId() const {
            return eventId;
        }

        void setEventId(int id) {
            eventId = id;
        }

        int getEntryCount() const {
            return entryCount;
        }

        void setEntryCount(int count) {
            entryCount = count;
        }

        int getEntriesLeft() const {
            return entriesLeft;
        }

        void setEntriesLeft(int entries) {
            entriesLeft = entries;
        }

        int getIsVIP() const {
            return isVIP;
        }

        void setIsVIP(int vip) {
            isVIP = vip;
        }

        int getAttendeeId() const {
            return attendeeId;
        }

        void setAttendeeId(int id) {
            attendeeId = id;
        }
};

#endif // TICKET_H;

