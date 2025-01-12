#ifndef DATESERVICES_H
#define DATESERVICES_H

#include <string>
#include <ctime>

time_t convertStringToTimeT(const std::string &dateString);
std::string convertTimeToString(time_t time);

#endif // DATESERVICES_H