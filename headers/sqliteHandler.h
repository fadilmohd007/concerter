#ifndef SQLITEHANDLER_H
#define SQLITEHANDLER_H

#include <string>
#include <sqlite3.h>

#include "event.h"
#include "attendee.h"
#include "ticket.h"

// EVENTS
static inline int getNextEventId(sqlite3 *db);
Event getEventByName(sqlite3 *db, std::string &eventName);
Event getEventById(sqlite3 *db, int eventId);
int saveEvent(sqlite3 *db, const Event &event);

// ATTENDEE
static inline int getNextAttendeeId(sqlite3 *db);
Attendee getAttendeeByName(sqlite3 *db, std::string &attendeeName);
Attendee getAttendeeById(sqlite3 *db, int attendeeId);
Attendee getAttendeeByEmail(sqlite3 *db, std::string &email);
int saveAttendee(sqlite3 *db, const Attendee &attendee);


// TICKETS
static inline int getNextTicketId(sqlite3 *db);
Ticket getTicketById(sqlite3 *db, int ticketId);
int saveTicket(sqlite3 *db, const Ticket &ticket);

#endif // SQLITEHANDLER_H