#include <iostream>
#include <stdexcept>
#include <string>
#include <sqlite3.h>
#include <ctime>

#include "event.h"
#include "dateServices.h"
#include "attendee.h"
#include "ticket.h"

Event getEventByName(sqlite3 *db, string &eventName)
{
    const char *select_sql = "SELECT eventId, eventName, eventStartDate, eventEndDate, eventLocation FROM Event WHERE eventName = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Event event;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the event name to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, eventName.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind event name");
    }

    // Execute the SQL statement and retrieve the event details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Event found with the given name.\n");
        event.setEventId(sqlite3_column_int(stmt, 0));
        string eventName ((const char *) sqlite3_column_text(stmt, 1));
        event.setEventName(eventName);
        event.setEventStartDate(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
        event.setEventEndDate(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
        event.setEventLocation(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    }
    else
    {
        printf("No event found with the given name.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No event found with the given name");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the event object
    return event;
}

Event getEventById(sqlite3 *db, int eventId)
{
    const char *select_sql = "SELECT eventId, eventName, eventStartDate, eventEndDate, eventLocation FROM Event WHERE eventId = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Event event;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the event ID to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, eventId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind event ID");
    }

    // Execute the SQL statement and retrieve the event details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Event found with the given ID.\n");
        event.setEventId(sqlite3_column_int(stmt, 0));
        event.setEventName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
        event.setEventStartDate(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
        event.setEventEndDate(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
        event.setEventLocation(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    }
    else
    {
        printf("No event found with the given ID.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No event found with the given ID");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the event object
    return event;
}

int getNextEventId(sqlite3 *db)
{
    int rc;
    sqlite3_stmt *stmt;
    int nextEventId = 1000; // Default starting event ID

    // Get the last eventId from the Event table
    const char *select_sql = "SELECT MAX(eventId) FROM Event;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1; // Return an error code
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL)
    {
        nextEventId = sqlite3_column_int(stmt, 0) + 1; // Increment the last eventId by 1
    }

    sqlite3_finalize(stmt);
    return nextEventId;
}

int saveEvent(sqlite3 *db, const Event &event)
{
    const char *insert_sql = "INSERT INTO Event (eventId, eventName, eventStartDate, eventEndDate, eventLocation) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int eventId;
    int ret;
    if(event.getEventId() != 0)
    {
        eventId = event.getEventId();
    }
    else
    {
        eventId = getNextEventId(db);
    }
    if (eventId == -1)
    {
        std::cerr << "Failed to get next event ID: " << sqlite3_errmsg(db) << std::endl;
        return SQLITE_ERROR;
    }

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // Bind the values to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, eventId); // Bind the event ID
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event ID: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 2, event.getEventName().c_str(), -1, SQLITE_STATIC); // Bind the event name
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event name: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 3, event.getEventStartDateAsString().c_str(), -1, SQLITE_STATIC); // Bind the event start date
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event start date: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 4, event.getEventEndDateAsString().c_str(), -1, SQLITE_STATIC); // Bind the event end date
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event end date: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 5, event.getEventLocation().c_str(), -1, SQLITE_STATIC); // Bind the event location
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event location: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    std::cout << "Event saved successfully." << std::endl;
    return SQLITE_OK;
}


//ATTENDEE

Attendee getAttendeeByName(sqlite3 *db, string &attendeeName)
{
    const char *select_sql = "SELECT attendeeId, attendeeName, attendeeEmail FROM Attendee WHERE attendeeName = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Attendee attendee;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the attendee name to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, attendeeName.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind attendee name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind attendee name");
    }

    // Execute the SQL statement and retrieve the attendee details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Attendee found with the given name.\n");
        attendee.setAttendeeId(sqlite3_column_int(stmt, 0));
        attendee.setAttendeeName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
        attendee.setAttendeeEmail(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
    }
    else
    {
        printf("No attendee found with the given name.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No attendee found with the given name");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the attendee object
    return attendee;
}

Attendee getAttendeeByEmail(sqlite3 *db, std::string &email)
{
    const char *select_sql = "SELECT attendeeId, attendeeName, attendeeEmail, attendeeMobileNum, groupName FROM Attendee WHERE attendeeEmail = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Attendee attendee;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the email to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind email: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind email");
    }

    // Execute the SQL statement and retrieve the attendee details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Attendee found with the given email.\n");
        attendee.setAttendeeId(sqlite3_column_int(stmt, 0));
        attendee.setAttendeeName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
        attendee.setAttendeeEmail(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
        attendee.setAttendeeMobileNum(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
        attendee.setGroupName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    }
    else
    {
        printf("No attendee found with the given email.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No attendee found with the given email");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the attendee object
    return attendee;
}

Attendee getAttendeeById(sqlite3 *db, int attendeeId)
{
    const char *select_sql = "SELECT attendeeId, attendeeName, attendeeEmail, attendeeMobileNum, groupName FROM Attendee WHERE attendeeId = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Attendee attendee;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the attendee ID to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, attendeeId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind attendee ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind attendee ID");
    }

    // Execute the SQL statement and retrieve the attendee details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Attendee found with the given ID.\n");
        attendee.setAttendeeId(sqlite3_column_int(stmt, 0));
        attendee.setAttendeeName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
        attendee.setAttendeeEmail(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
        attendee.setAttendeeMobileNum(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
        attendee.setGroupName(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    }
    else
    {
        printf("No attendee found with the given ID.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No attendee found with the given ID");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the attendee object
    return attendee;
}

int getNextAttendeeId(sqlite3 *db)
{
    int rc;
    sqlite3_stmt *stmt;
    int nextAttendeeId = 1000; // Default starting attendee ID

    // Get the last attendeeId from the Attendee table
    const char *select_sql = "SELECT MAX(attendeeId) FROM Attendee;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1; // Return an error code
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL)
    {
        nextAttendeeId = sqlite3_column_int(stmt, 0) + 1; // Increment the last attendeeId by 1
    }

    sqlite3_finalize(stmt);
    return nextAttendeeId;
}

int saveAttendee(sqlite3 *db, const Attendee &attendee)
{
    const char *insert_sql = "INSERT INTO Attendee (attendeeId, attendeeName, attendeeEmail, attendeeMobileNum, groupName) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int attendeeId;
    int ret;
    if(attendee.getAttendeeId() != 0)
    {
        attendeeId = attendee.getAttendeeId();
    }
    else
    {
        attendeeId = getNextAttendeeId(db);
    }
    if (attendeeId == -1)
    {
        std::cerr << "Failed to get next attendee ID: " << sqlite3_errmsg(db) << std::endl;
        return SQLITE_ERROR;
    }

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return SQLITE_ERROR;
    }

    // Bind the values to the prepared statement
    sqlite3_bind_int(stmt, 1, attendeeId);
    sqlite3_bind_text(stmt, 2, attendee.getAttendeeName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, attendee.getAttendeeEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, attendee.getAttendeeMobileNum().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, attendee.getGroupName().c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to insert attendee: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return SQLITE_ERROR;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

//TICKETS
static inline int getNextTicketId(sqlite3 *db)
{
    int rc;
    sqlite3_stmt *stmt;
    int nextTicketId = 1000; // Default starting ticket ID

    // Get the last ticketId from the Ticket table
    const char *select_sql = "SELECT MAX(ticketId) FROM Ticket;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1; // Return an error code
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL)
    {
        nextTicketId = sqlite3_column_int(stmt, 0) + 1; // Increment the last ticketId by 1
    }

    sqlite3_finalize(stmt);
    return nextTicketId;
}

Ticket getTicketById(sqlite3 *db, int ticketId)
{
    const char *select_sql = "SELECT ticketId, entryCount, entriesLeft, isVIP, attendeeId, eventId FROM Ticket WHERE ticketId = ?;";
    sqlite3_stmt *stmt;
    int rc;
    Ticket ticket;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        throw std::runtime_error("Failed to prepare statement");
    }

    // Bind the ticket ID to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, ticketId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind ticket ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind ticket ID");
    }

    // Execute the SQL statement and retrieve the ticket details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Ticket found with the given ID.\n");
        ticket.ticketId = sqlite3_column_int(stmt, 0);
        ticket.entryCount = sqlite3_column_int(stmt, 1);
        ticket.entriesLeft = sqlite3_column_int(stmt, 2);
        ticket.isVIP = sqlite3_column_int(stmt, 3);
        ticket.attendeeId = sqlite3_column_int(stmt, 4);
        ticket.eventId = sqlite3_column_int(stmt, 5);
    }
    else
    {
        printf("No ticket found with the given ID.\n");
        sqlite3_finalize(stmt);
        throw std::runtime_error("No ticket found with the given ID");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return the ticket object
    return ticket;
}

int saveTicket(sqlite3 *db, const Ticket &ticket)
{
    const char *insert_sql = "INSERT INTO Ticket (ticketId, entryCount, entriesLeft, isVIP, attendeeId, eventId) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int ticketId;
    int ret;
    if(ticket.ticketId != 0)
    {
        ticketId = ticket.ticketId;
    }
    else
    {
        ticketId = getNextTicketId(db);
    }
    if (ticketId == -1)
    {
        std::cerr << "Failed to get next ticket ID: " << sqlite3_errmsg(db) << std::endl;
        return SQLITE_ERROR;
    }

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // Bind the values to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, ticketId); // Bind the ticket ID
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind ticket ID: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 2, ticket.entryCount); // Bind the entry count
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind entry count: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 3, ticket.entriesLeft); // Bind the entries left
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind entries left: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 4, ticket.isVIP); // Bind the VIP status
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind VIP status: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 5, ticket.attendeeId); // Bind the attendee ID
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind attendee ID: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 6, ticket.eventId); // Bind the event ID
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to bind event ID: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    std::cout << "Ticket saved successfully." << std::endl;
    return SQLITE_OK;
}