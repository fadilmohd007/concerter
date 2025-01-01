#include <iostream>
#include "attendee.h"

class Ticket 
{
    public:
        int VerifyTicket();
        int ticketId;
        int eventId;
        //Number of entries made using this ticket
        int entryCount {0};
        int entriesLeft;
        int isVIP;
        Attendee attendee;
};

