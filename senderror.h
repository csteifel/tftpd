#ifndef SENDINGERROR_H
#define SENDINGERROR_H

#include <string>
#include <sys/socket.h>

int sendError(int, std::string);

#endif