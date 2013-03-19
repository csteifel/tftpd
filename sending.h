#ifndef SENDING_H
#define SENDING_H

#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>

#include "senderror.h"

void sendFile(int, sockaddr_storage, socklen_t, char *);


#endif