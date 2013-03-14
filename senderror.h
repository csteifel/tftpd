#ifndef SENDINGERROR_H
#define SENDINGERROR_H

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

void sendError(int, sockaddr_storage, socklen_t, unsigned short, std::string);

#endif