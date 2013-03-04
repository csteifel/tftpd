#ifndef SENDINGERROR_H
#define SENDINGERROR_H

#include <string>
#include <sys/socket.h>

void sendError(int, sockaddr_storage, socklen_t, std::string);

#endif