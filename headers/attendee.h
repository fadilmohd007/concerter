#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef ATTENDEE_H
#define ATTENDEE_H

class Attendee 
{
    string name;
    string email;
    string mobileNum;
    public:
        void printDetails() const;
};
#endif // ATTENDEE_H