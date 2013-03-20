#ifndef RECEIVING_H
#define RECEIVING_H

#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "senderror.h"

void receiveFile(int, sockaddr_storage, socklen_t, char *);


#endif