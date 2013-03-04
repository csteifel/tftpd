#include "senderror.h"


void sendError(int sockfd, sockaddr_storage sendErrTo, socklen_t sendErrToLen, std::string errorString){
	sendto(sockfd, errorString.c_str(), errorString.length(), 0, (struct sockaddr *) &sendErrTo, sendErrToLen);
}