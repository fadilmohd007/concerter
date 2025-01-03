#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>


#include "sqlite_conn.h"

const char *sqlTicketTableCreate =
    "CREATE TABLE IF NOT EXISTS Ticket ("
    "ticketId INTEGER PRIMARY KEY, "
    "eventId INTEGER, "
    "entryCount INTEGER, "
    "entriesLeft INTEGER, "
    "isVIP INTEGER, "
    "attendeeId INTEGER, "
    "FOREIGN KEY(eventId) REFERENCES Event(eventId), "
    "FOREIGN KEY(attendeeId) REFERENCES Attendee(attendeeId));";

const char *sqlEventTableCreate =
    "CREATE TABLE IF NOT EXISTS Event ("
    "eventId INTEGER PRIMARY KEY, "
    "eventName TEXT UNIQUE, "
    "eventStartDate TEXT, "
    "eventEndDate TEXT, "
    "eventLocation TEXT);";


const char *sqlAttendeeTableCreate = 
    "CREATE TABLE IF NOT EXISTS Attendee ("
    "attendeeId INTEGER PRIMARY KEY, "
    "name TEXT, "
    "email TEXT UNIQUE, "
    "mobileNum TEXT UNIQUE, "
    "groupName TEXT);";

// GENERIC
int CreateDB(char *dbName, sqlite3 **db)
{
    char *err_msg = 0;
    int rc;

    // Open the database or create it if it doesn't exist
    rc = sqlite3_open(dbName, db); // "local.db" is the database file name

    if (rc)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return -1;
    }
    else
    {
        printf("Opened database successfully\n");
        return 0;
    }
}

int DBExec(const char *dbQeury, sqlite3 *db)
{
    char *err_msg = 0;
    int rc;
    rc = sqlite3_exec(db, dbQeury, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    else
    {
        printf("Exec ran successfully \n");
        return 0;
    }
}

// EVENTS

int getNextEventId(sqlite3 *db, int *eventId)
{
    int rc;
    sqlite3_stmt *stmt;
    // Get the last eventId from the Event table
    const char *select_sql = "SELECT MAX(eventId) FROM Event;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) != 0)
    {
        *eventId = sqlite3_column_int(stmt, 0) + 1; // Increment the last eventId by 1
    }
    else
    {
        *eventId = 1000; // Start from 1000 if no rows are found
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int SaveEvent(sqlite3 *db, const char *eventName, const char *eventStartDate, const char *eventEndDate, const char *eventLocation)
{
    const char *insert_sql = "INSERT INTO Event (eventId, eventName, eventStartDate, eventEndDate, eventLocation) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int eventId;
    getNextEventId(db, &eventId);
    printf("eventId %d\n", eventId); // Starting event ID
    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the values to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, eventId); // Bind the event ID
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 2, eventName, -1, SQLITE_STATIC); // Bind the event name
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 3, eventStartDate, -1, SQLITE_STATIC); // Bind the event date
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event date: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 4, eventEndDate, -1, SQLITE_STATIC); // Bind the event end date
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event end date: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 5, eventLocation, -1, SQLITE_STATIC); // Bind the event location
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event location: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Increment the event ID for the next entry
    eventId++;

    // Return success
    printf("Event saved successfully.\n");
    return SQLITE_OK;
}

int GetEventById(sqlite3 *db, event_t *event,  int eventId)
{
    const char *select_sql = "SELECT eventId, eventName, eventStartDate, eventEndDate, eventLocation FROM Event WHERE eventId = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the event ID to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, eventId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement and retrieve the event details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Event found with the given ID.\n");
        event->eventId = sqlite3_column_int(stmt, 0);
        const char *eventName = (const char *)sqlite3_column_text(stmt, 1);
        strcpy(event->eventName, eventName);

        const char *startDate = (const char *)sqlite3_column_text(stmt, 2);
        strcpy(event->eventStartDate, startDate);

        const char *endDate = (const char *)sqlite3_column_text(stmt, 3);
        strcpy(event->eventEndDate, endDate);

        const char *location = (const char *)sqlite3_column_text(stmt, 4);
        strcpy(event->eventLocation, location);
    }
    else
    {
        printf("No event found with the given ID.\n");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    return SQLITE_OK;
}


int GetEventByName(sqlite3 *db, const char *eventName)
{
    const char *select_sql = "SELECT eventId, eventName, eventStartDate, eventEndDate, eventLocation FROM Event WHERE eventName = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the event name to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, eventName, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement and retrieve the event details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Event found with the given name.\n");
        // printf("Event ID: %d\n", sqlite3_column_int(stmt, 0));
        // printf("Event Name: %s\n", sqlite3_column_text(stmt, 1));
        // printf("Event Start Date: %s\n", sqlite3_column_text(stmt, 2));
        // printf("Event End Date: %s\n", sqlite3_column_text(stmt, 3));
        // printf("Event Location: %s\n", sqlite3_column_text(stmt, 4));
    }
    else
    {
        printf("No event found with the given name.\n");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    return SQLITE_OK;
}

// ATTENDEE
int getNextAttendeeId(sqlite3 *db, int *attendeeId)
{
    int rc;
    sqlite3_stmt *stmt;
    // Get the last attendeeId from the Attendee table
    const char *select_sql = "SELECT MAX(attendeeId) FROM Attendee;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) != 0)
    {   
        *attendeeId = sqlite3_column_int(stmt, 0) + 1; // Increment the last attendeeId by 1
    }
    else
    {
        printf("starting\n");
        *attendeeId = 1000; // Start from 1000 if no rows are found
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int SaveAttendee(sqlite3 *db, const char *name, const char *email, const char *mobileNum, const char* groupName)
{
    const char *insert_sql = "INSERT INTO Attendee (attendeeId, name, email, mobileNum, groupName) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int attendeeId;

    // Get the next attendee ID
    getNextAttendeeId(db,&attendeeId);
    printf("attendeeId %d\n",attendeeId);

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the values to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, attendeeId); // Bind the attendee ID
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind attendee ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC); // Bind the name
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC); // Bind the email
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind email: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 4, mobileNum, -1, SQLITE_STATIC); // Bind the mobile number
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind mobile number: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    
    if (groupName == NULL || strcmp(groupName, "") || strcmp(groupName, "\n") == 0)
    {
        groupName = "no_group\n"; // Set groupName to "no_group" if it is NULL or empty
    }

    rc = sqlite3_bind_text(stmt, 5, groupName, -1, SQLITE_STATIC); // Bind the group Name
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind group ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    printf("Attendee saved successfully.\n");
    return SQLITE_OK;
}

int SaveAttendeeFromCSV( sqlite3 *db, const char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file: %s\n", filePath);
        return -1;
    }

    char line[1024];
    // Skip the header line
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file))
    {
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *mobileNum = strtok(NULL, ",");
        char *groupName = strtok(NULL, ",");

        if (name && email && mobileNum && groupName)
        {
            printf("Name: %s, Email: %s, Mobile Number: %s, Group Name: %s\n", name, email, mobileNum, groupName);
            SaveAttendee(db, name, email, mobileNum, groupName);
        }
    }

    fclose(file);
    return 0;
}


int GetAttendeeById(sqlite3 *db, int attendeeId)
{
    const char *select_sql = "SELECT attendeeId, name, email, mobileNum, groupName FROM Attendee WHERE attendeeId = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the attendee ID to the prepared statement
    rc = sqlite3_bind_int(stmt, 1, attendeeId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind attendee ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement and retrieve the attendee details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Attendee found with the given ID:%d.\n", attendeeId);
        // printf("Attendee ID: %d\n", sqlite3_column_int(stmt, 0));
        // printf("Name: %s\n", sqlite3_column_text(stmt, 1));
        // printf("Email: %s\n", sqlite3_column_text(stmt, 2));
        // printf("Mobile Number: %s\n", sqlite3_column_text(stmt, 3));
        // printf("Group ID: %d\n", sqlite3_column_int(stmt, 4));
    }
    else
    {
        printf("No attendee found with the given ID.\n");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    return SQLITE_OK;
}

int GetAttendeeByName(sqlite3 *db, const char *name)
{
    const char *select_sql = "SELECT attendeeId, name, email, mobileNum, groupName FROM Attendee WHERE name = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind the name to the prepared statement
    rc = sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind name: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    // Execute the SQL statement and retrieve the attendee details
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Attendee found with the given name:%s.\n", name);
        // printf("Attendee ID: %d\n", sqlite3_column_int(stmt, 0));
        // printf("Name: %s\n", sqlite3_column_text(stmt, 1));
        // printf("Email: %s\n", sqlite3_column_text(stmt, 2));
        // printf("Mobile Number: %s\n", sqlite3_column_text(stmt, 3));
        // printf("Group ID: %d\n", sqlite3_column_int(stmt, 4));
    }
    else
    {
        printf("No attendee found with the given name.\n");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    // Return success
    return SQLITE_OK;
}


//TICKETS
int getNextTicketId(sqlite3 *db, int *ticketId)
{
    int rc;
    sqlite3_stmt *stmt;
    const char *select_sql = "SELECT MAX(ticketId) FROM Ticket;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) != 0)
    {
        *ticketId = sqlite3_column_int(stmt, 0) + 1;
    }
    else
    {
        *ticketId = 1000;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int SaveTicket(sqlite3 *db, int eventId, int entryCount, int entriesLeft, int isVIP, int attendeeId)
{
    const char *insert_sql = "INSERT INTO Ticket (ticketId, eventId, entryCount, entriesLeft, isVIP, attendeeId) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int ticketId;

    getNextTicketId(db, &ticketId);
    printf("ticketId %d\n", ticketId);

    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 1, ticketId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind ticket ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 2, eventId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 3, entryCount);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind entry count: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 4, entriesLeft);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind entries left: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 5, isVIP);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind VIP status: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 6, attendeeId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind attendee ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    printf("Ticket saved successfully.\n");
    return SQLITE_OK;
}

int GetTicketById(sqlite3 *db, int ticketId)
{
    const char *select_sql = "SELECT ticketId, eventId, entryCount, entriesLeft, isVIP, attendeeId FROM Ticket WHERE ticketId = ?;";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 1, ticketId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind ticket ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Ticket found with the given ID.\n");
    }
    else
    {
        printf("No ticket found with the given ID.\n");
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int GetTicketByEventId(sqlite3 *db, int eventId)
{
    const char *select_sql = "SELECT ticketId, eventId, entryCount, entriesLeft, isVIP, attendeeId FROM Ticket WHERE eventId = ?;";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 1, eventId);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to bind event ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        printf("Ticket found with the given event ID.\n");
    }
    else
    {
        printf("No ticket found with the given event ID.\n");
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

//GENERAL
int InitAllTables(sqlite3 *db)
{
    int rc;
    rc = DBExec(sqlAttendeeTableCreate, db);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    rc = DBExec(sqlEventTableCreate, db);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    rc = DBExec(sqlTicketTableCreate, db);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    return SQLITE_OK;
}