#ifndef SENDING_H
#define SENDING_H

#include <string>
#include <iostream>
#include <sys/socket.h>

int sendFile(int, sockaddr_storage, socklen_t, std::string filename);

#endif