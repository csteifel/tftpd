#ifndef SENDING_H
#define SENDING_H

#include <string>
#include <iostream>
#include <sys/socket.h>

int sendFile(std::string filename);

#endif