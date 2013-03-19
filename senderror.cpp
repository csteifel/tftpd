#include "senderror.h"

/**
 * Sends an error to the client
 * @param sockfd       The socket that the message is to be sent on (INVALID AFTER FUNCTION EXECUTION)
 * @param sendErrTo    sockaddr information needed for addressing
 * @param sendErrToLen sockaddr length information
 * @param errorCode    The error code to send to the client
 * @param errorString  The errror message to send to the client
 */
void sendError(int sockfd, sockaddr_storage sendErrTo, socklen_t sendErrToLen, uint16_t errorCode, std::string errorString){
	unsigned int bufSize = 2 + 2 + errorString.length() + 1;
	char * errorBuffer = (char *) malloc(bufSize);
	uint16_t opcode = htons(5);
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