#include <iostream>
#include <cctype>
#include <algorithm>


#include "sqlite_conn.h"
#include "program.h"

using namespace std;


int main(int argc, char* argv[])
{
    string programName;
    sqlite3 *db;
    int ret,option;


    cout << "welcome to concerter" << "\n";
    cout << "please enter the program name:" << "\n";
    cin >> programName;
    cout << "creating DB for your program " << programName << "\n";
    Program program(programName);
    CreateDB(program.programName.data(), &db);
    ret = InitAllTables(db);
    if(ret < 0)
    {
        cout << "Failed to create tables" << "\n";
        exit(EXIT_FAILURE);
    }
    std::string eventName,eventDate, eventEndDate, eventLocation;
    std::string attendeeName, attendeeEmail;
    std::string attendeeMobile;

    int x = 2;
    enter_option:
    option = 0;
    cout << "select an option to do :\n\
            1. Add Event\n\
            2. Add Attendee\n\
            3. Add Ticket\n\
            4. Verify Ticket\n\
            5. Exit\n";
    cin >> option;
    printf("option selected: %d\n", option);
    switch (option)
    {
    case 1:
        cout << "Enter eventName" << "\n";
        cin >> eventName;
        cout << "Enter eventDate" << "\n";
        cin >> eventDate;
        cout << "Enter eventEndDate" << "\n";
        cin >> eventEndDate;
        cout << "Enter eventLocation" << "\n";
        cin >> eventLocation;
        cout << "Saving Event Details:" << "\n";
        cout << "Name: " << eventName << "\n";
        cout << "Date: " << eventDate << "\n";
        cout << "End Date: " << eventEndDate << "\n";
        cout << "Location: " << eventLocation << "\n";
        SaveEvent(db, eventName.c_str(), eventDate.c_str(), eventEndDate.c_str(), eventLocation.c_str());
        GetEventById(db, 1000);
        GetEventByName(db, "concert_01_01");

        break;
    case 2:
        {
            cout << "Enter attendeeName" << "\n";
            cin >> attendeeName;
            cout << "Enter attendeeEmail" << "\n";
            cin >> attendeeEmail;
            cout << "Enter attendeeMobile (10 digits)" << "\n";
            cin >> attendeeMobile;
            while (attendeeMobile.length() != 10 || !std::all_of(attendeeMobile.begin(), attendeeMobile.end(), ::isdigit)) {
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
            std::string ticketType, ticketPrice;
            cout << "Enter ticketType" << "\n";
            cin >> ticketType;
            cout << "Enter ticketPrice" << "\n";
            cin >> ticketPrice;
            while (!std::all_of(ticketPrice.begin(), ticketPrice.end(), ::isdigit)) {
                cout << "Invalid price. Please enter a valid ticket price:" << "\n";
                cin >> ticketPrice;
            }
            cout << "Saving Ticket Details:" << "\n";
            cout << "Type: " << ticketType << "\n";
            cout << "Price: " << ticketPrice << "\n";
            SaveTicket(db, ticketType.c_str(), std::stoi(ticketPrice));
            // GetTicketById(db, 1000);
            // GetTicketByType(db, "VIP");
            break;
        }
    default:
        break;
    }
    x--;
    if(x > 0)
        goto enter_option;
    

    
}