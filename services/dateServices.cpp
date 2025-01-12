#include "dateServices.h"
#include <iostream>
#include <ctime>

time_t convertStringToTimeT(const std::string &dateString)
{
    struct tm tm;
    if (strptime(dateString.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL)
    {
        std::cerr << "Error parsing date string" << std::endl;
        return -1;
    }
    return mktime(&tm);
}


std::string convertTimeToString(time_t time)
{
    struct tm *tm = localtime(&time);
    char buffer[20];
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm) == 0)
    {
        std::cerr << "Error formatting time" << std::endl;
        return "";
    }
    return std::string(buffer);
}