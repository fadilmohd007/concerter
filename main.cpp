#include <iostream>
#include <cctype>
#include <algorithm>

#include "sqlite_conn.h"
#include "program.h"
#include "event.h"
#include "attendee.h"

using namespace std;

int main(int argc, char *argv[])
{
    string programName;
    sqlite3 *db;
    int ret, option;

    cout << "welcome to concerter" << "\n";
    cout << "please enter the program name:" << "\n";
    cin >> programName;
    cout << "creating DB for your program " << programName << "\n";
    Program program(programName);
    CreateDB(program.programName.data(), &db);
    ret = InitAllTables(db);
    if (ret < 0)
    {
        cout << "Failed to create tables" << "\n";
        exit(EXIT_FAILURE);
    }
    std::string eventName, eventStartDate, eventStartTime, eventEndDate, eventEndTime, eventLocation;
    std::string attendeeName, attendeeEmail;
    std::string attendeeMobile;
    Event event;
    int x = 1;
    cout << "Enter the number of operations you want to perform:" << "\n";
    cin >> x;

enter_option:
    option = 0;
    cout << "select an option to do :\n\
            1. Add Event\n\
            2. Add Attendee\n\
            3. Add Ticket\n\
            4. Add Attendee from CSV\n\
            5. Exit\n";
    cin >> option;
    printf("option selected: %d\n", option);
    switch (option)
    {
    case 1:
        cout << "Enter eventName" << "\n";
        cin >> eventName;
        cout << "Enter eventStartDate" << "\n";
        while (true)
        {
            cout << "Enter eventStartDate (format: %Y-%m-%d %H:%M:%S)" << "\n";
            cin >> eventStartDate;
            cin >> eventStartTime;
            eventStartDate += " " + eventStartTime;
            struct tm tm;
            if (strptime(eventStartDate.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
            {
                break;
            }
            else
            {
                cout << "Invalid date format. Please try again." << "\n";
            }
        }
        while (true)
        {
            cout << "Enter eventEndDate (format: %Y-%m-%d %H:%M:%S)" << "\n";
            cin >> eventEndDate;
            cin >> eventEndTime;
            eventEndDate += " " + eventEndTime;
            struct tm tm;
            if (strptime(eventEndDate.c_str(), "%Y-%m-%d %H:%M:%S", &tm))
            {
                break;
            }
            else
            {
                cout << "Invalid date format. Please try again." << "\n";
            }
        }

        cout << "Enter eventLocation" << "\n";
        cin >> eventLocation;
        cout << "Saving Event Details:" << "\n";
        cout << "Name: " << eventName << "\n";
        cout << "Date: " << eventStartDate << "\n";
        cout << "End Date: " << eventEndDate << "\n";
        cout << "Location: " << eventLocation << "\n";
        SaveEvent(db, eventName.c_str(), eventStartDate.c_str(), eventEndDate.c_str(), eventLocation.c_str());

        event.getEventById(1000, db);
        event.printEvent();
        // GetEventByName(db, "concert_01_01");

        break;
    case 2:
    {
        cout << "Enter attendeeName" << "\n";
        cin >> attendeeName;
        cout << "Enter attendeeEmail" << "\n";
        cin >> attendeeEmail;
        cout << "Enter attendeeMobile (10 digits)" << "\n";
        cin >> attendeeMobile;
        while (attendeeMobile.length() != 10 || !std::all_of(attendeeMobile.begin(), attendeeMobile.end(), ::isdigit))
        {
            cout << "Invalid mobile number. Please enter a 10-digit mobile number:" << "\n";
            cin >> attendeeMobile;
        }
        cout << "Saving Attendee Details:" << "\n";
        cout << "Name: " << attendeeName << "\n";
        cout << "Email: " << attendeeEmail << "\n";
        cout << "Mobile: " << attendeeMobile << "\n";
        SaveAttendee(db, attendeeName.c_str(), attendeeEmail.c_str(), attendeeMobile.c_str(), 0);
        GetAttendeeById(db, 1000);
        GetAttendeeByName(db, "JohnDoe");
        break;
    }
    case 3:
    {
        int ticketId;
        string eventName;
        int entryCount = 0;
        int entriesLeft;
        int isVIP;

        cout << "Enter eventName" << "\n";
        cin >> eventName;
        cout << "Enter entryCount" << "\n";
        cin >> entryCount;
        cout << "Enter entriesLeft" << "\n";
        cin >> entriesLeft;
        cout << "Is VIP (1 for Yes, 0 for No)" << "\n";
        cin >> isVIP;
        cout << "Enter attendeeEmail" << "\n";
        cin >> attendeeEmail;
        GetEventByName(db, eventName.c_str());
        // int eventId = GetEventByName(db, eventName.c_str());
        // if (eventId < 0) {
        //     cout << "Event not found. Please check the event name and try again." << "\n";
        //     break;
        // }
        // attendee = GetAttendeeByEmail(db, attendeeEmail.c_str());
        // if (attendee.id < 0) {
        //     cout << "Attendee not found. Please check the attendee email and try again." << "\n";
        //     break;
        // }
        // ticketId = GenerateTicketId(); // Assuming you have a function to generate a unique ticket ID
        // cout << "Saving Ticket Details:" << "\n";
        // cout << "Ticket ID: " << ticketId << "\n";
        // cout << "Event ID: " << eventId << "\n";
        // cout << "Entry Count: " << entryCount << "\n";
        // cout << "Entries Left: " << entriesLeft << "\n";
        // cout << "Is VIP: " << isVIP << "\n";
        // cout << "Attendee Name: " << attendeeName << "\n";
        // cout << "Attendee Email: " << attendeeEmail << "\n";
        // cout << "Attendee Mobile: " << attendeeMobile << "\n";
        // SaveTicket(db, ticketId, eventId, entryCount, entriesLeft, isVIP, attendee);
        // GetTicketById(db, ticketId);
        // GetTicketByEventId(db, eventId);
        break;
    }
    case 4:
    {
        string filePath;
        cout << "Enter the file path for attendee CSV:" << "\n";
        cin >> filePath;
        SaveAttendeeFromCSV(db, filePath.c_str());
        break;
    }
    default:
        break;
    }
    x--;
    if (x > 0)
        goto enter_option;
}