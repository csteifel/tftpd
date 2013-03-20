#include "receiving.h"

void receiveFile(int clientFd, sockaddr_storage recvFrom, socklen_t recvFromLen, char * fname){
	char data[516], ack[4];

	struct stat statBuf;

	std::string filename(fname);
	free(fname);

	//Check if file exists first and if it does we send an error
	
	if(stat(filename.c_str(), &statBuf) != -1){
		//If stat didn't fail then the file exists already
		sendError(clientFd, recvFrom, recvFromLen, 6, "File already exists.");
		return;
	}
	
	std::ofstream file(filename.c_str(), std::ifstream::out | std::ifstream::binary);

	uint16_t blockNum, currentBlockNum = 0;
	uint16_t ackOpcode, recvCode;
	int amountSent, recvAmount;

	ackOpcode = htons(4);
	blockNum = htons(currentBlockNum);

	//Set up buffers to wait for data
	memset(data, 0, 516);
	memset(ack, 0, 4);

	memcpy(ack, &ackOpcode, 2);
	memcpy(ack, &blockNum, 2);


	bool receivedAll = false;

	do{
		//Send ack, start out with ack to take into account sending ack with block number 0.
		amountSent = sendto(clientFd, ack, 4, 0, (struct sockaddr *) &recvFrom, recvFromLen);

		if(amountSent != 4){
			std::cerr << "There was an error sending the packet! Supposed to send " << sendAmount << " actually sent " << actuallySent << "\n";
			std::cerr << "Error code: " << errno << " error string: " << strerror(errno) << "\n";
			close(clientFd);
			return;
		}

		recvAmount = recvFrom(clientFd, data, 516, 0, NULL, NULL);

		if(recvAmount == -1){
			//There was an error with receiving
			std::cerr << "There was an error sending the packet! Supposed to send " << sendAmount << " actually sent " << actuallySent << "\n";
			std::cerr << "Error code: " << errno << " error string: " << strerror(errno) << "\n";
		}else{
			//This is the last packet that we are going to recieve
			memcpy(&recvCode, data, 2);
			recvCode = ntohs(recvCode);
			if(recvCode == 5){
				//the client sent an error packet we can close connection
				std::cerr << "Error recieved!";
				//TODO: add in display for what error was received
				close(clientFd);
				return;
			}

			if(recvAmount < 516){
				//We are done receiving packets just send the last ack
				
				recievedAll = true;	
			}


			
		}


	}while(receivedAll == false);

	//Send the last ack message to the client


	close(clientFd);
}