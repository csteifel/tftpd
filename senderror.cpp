#include "senderror.h"


void sendError(int sockfd, sockaddr_storage sendErrTo, socklen_t sendErrToLen, unsigned short errorCode, std::string errorString){
	unsigned int bufSize = 2 + 2 + errorString.length() + 1;
	char * errorBuffer = (char *) malloc(bufSize);
	unsigned short opcode = htons(5);
	errorCode = htons(errorCode);

	//Zero everything out and then copy over all the information we need to send
	memset(errorBuffer, 0, bufSize);

	memcpy(errorBuffer, &opcode, 2);
	memcpy(errorBuffer+2, &errorCode, 2);
	memcpy(errorBuffer+4, errorString.c_str(), errorString.length());

	if(sendto(sockfd, errorBuffer, bufSize, 0, (struct sockaddr *) &sendErrTo, sendErrToLen) == -1){
		std::cerr << "Error sending error packet!";
	}
	//Done with the socket since there was an error
	close(sockfd);
}