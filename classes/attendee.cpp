
#include <iostream>


#include "attendee.h"

using namespace std;

void Attendee::printDetails() const
{
    std::cout << "Name: " << name << ", Email: " << email << ", Mobile Number: " << mobileNum << std::endl;
}

