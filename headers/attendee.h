#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef ATTENDEE_H
#define ATTENDEE_H

class Attendee
{
    int attendeeId;
    string name;
    string email;
    string mobileNum;
    string groupName;

public:
    void printDetails() const;

    int getAttendeeId() const { return attendeeId; }
    string getAttendeeName() const { return name; }
    string getAttendeeEmail() const { return email; }
    string getAttendeeMobileNum() const { return mobileNum; }
    string getGroupName() const { return groupName; }

    void setAttendeeId(int id)
    {
        attendeeId = id;
    }

    void setAttendeeName(const std::string &name)
    {
        this->name = name;
    }

    void setAttendeeEmail(const std::string &email)
    {
        this->email = email;
    }

    void setAttendeeMobileNum(const std::string &mobileNum)
    {
        this->mobileNum = mobileNum;
    }

    void setGroupName(const std::string &groupName)
    {
        this->groupName = groupName;
    }
};
#endif // ATTENDEE_H