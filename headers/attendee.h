#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef ATTENDEE_H
#define ATTENDEE_H

class Group
{
    vector <Attendee> group;
    string groupName;
};


class Attendee 
{
    string name;
    string email;
    string mobileNum;
};
#endif // ATTENDEE_H