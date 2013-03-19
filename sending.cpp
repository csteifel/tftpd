#include "sending.h"

/**
 * Send the file to the client
 * @param clientFd     The socket that was created to send information to the client on
 * @param sendingTo    Sockaddr structure containing client information
 * @param sendingToLen The length of the sockaddr structure
 * @param file         sendFile is responsible for cleaning up this memory allocation
 */
void sendFile(int clientFd, sockaddr_storage sendingTo, socklen_t sendingToLen, char * fname){
	//Data packet to send
	char data[516], recvBuf[4];

	uint16_t opcode = htons(3);
	uint16_t blockNumberHost = 1;
	uint16_t blockNumber = htons(blockNumberHost);
	bool doneProcessing = false, receivedAck;
	int actuallySent, amountReceived;
	size_t sendAmount;

	struct timeval timeoutValue;

	std::string filename(fname);
	free(fname);


	memset(data, 0, 516);
	memcpy(data, &opcode, 2);

	memset(recvBuf, 0, 4);

	//Only supporting files in binary mode
	std::ifstream file(filename, std::ifstream::in | std::ifstream::binary);

	if(file.is_open() == false){
		sendError(clientFd, sendingTo, sendingToLen, 1, "File not found.");
		return;
	}


	//Set a timeout for recieving messages so that we can wait a time amount and resend if need be
	timeoutValue.tv_sec = 10; //Value in seconds
	setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &timeoutValue, sizeof(struct timeval));


	while(file.good() && doneProcessing == false){

		memcpy(data+2, &blockNumber, 2);

		file.read(data+4, 512);
		sendAmount = file.gcount();
		
		//Send amount + 4 to account for opcode and block number
		sendAmount += 4;

		receivedAck = false;

		do{
			actuallySent = sendto(clientFd, data, sendAmount, 0, (struct sockaddr *) &sendingTo, sendingToLen);

			if(actuallySent == -1){
				std::cerr << "There was an error sending the packet! Supposed to send " << sendAmount << " actually sent " << actuallySent << "\n";
				std::cerr << "Error code: " << errno << " error string: " << strerror(errno) << "\n";
				return;
			}

			amountReceived = recvfrom(clientFd, recvBuf, 4, 0, NULL, NULL);
			
			
			//Wait for ACK from client
			if(amountReceived > 0){
				uint16_t recvCode;
				memcpy(&recvCode, recvBuf, 2);
				uint16_t recvBlock;
				memcpy(&recvBlock, recvBuf+2, 2);

				recvCode = ntohs(recvCode);
				recvBlock = ntohs(recvBlock);

				if(recvCode == 4 && recvBlock == blockNumberHost){
					//Recieved the proper acknowledgement
					if(file.eof()){
						doneProcessing = true;
						break;
					}
					receivedAck = true;
				}else if(recvCode == 5){
					//If recieved an error packet then the RFC says the connection can be closed
					close(clientFd);
					return;
				}else{
					//Recieved some other sort of packet that we should not have.
					sendError(clientFd, sendingTo, sendingToLen, 4, "Illegal TFTP operation.");
					return;
				}
			}else{
				//Did not recieve the proper acknowledgement if timed out then we need to resend
				if(errno == EWOULDBLOCK || errno == EAGAIN){
					//Just a timeout so we need to resend
					std::cerr << "Timed out\n";
				}else{
					std::cerr << "Error number: " << errno << " error string: " << strerror(errno) << "\n";
					sendError(clientFd, sendingTo, sendingToLen, 0, "Socket error!");
					return;
				}
			}
		}while(receivedAck == false);

		//Move onto the next block
		blockNumber = htons(++blockNumberHost);
	}


}