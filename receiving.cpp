#include "receiving.h"

/**
 * Receive a file from the client
 * @param clientFd    The socket that was created to send/recv information to the client on
 * @param recvFrom    Sockaddr structure containing client information
 * @param recvFromLen The length of the sockaddr structure
 * @param fname       receiveFile is responsible for freeing this memory
 */
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
	blockNum = htons(currentBlockNum); //Initially set to 0 to send original ack
	++currentBlockNum;

	//Set up buffers to wait for data
	memset(ack, 0, 4);

	memcpy(ack, &ackOpcode, 2);

	bool receivedLast = false;

	do{
		memset(data, 0, 516);
		//Send ack, start out with ack to take into account sending ack with block number 0.
		memcpy(ack+2, &blockNum, 2);
		amountSent = sendto(clientFd, ack, 4, 0, (struct sockaddr *) &recvFrom, recvFromLen);

		if(amountSent != 4){
			std::cerr << "There was an error sending the packet!\n";
			std::cerr << "Error code: " << errno << " error string: " << strerror(errno) << "\n";
			close(clientFd);
			return;
		}

		recvAmount = recvfrom(clientFd, data, 516, 0, NULL, NULL);

		if(recvAmount == -1){
			//There was an error with receiving
			std::cerr << "There was an error receiving the packet!\n";
			std::cerr << "Error code: " << errno << " error string: " << strerror(errno) << "\n";
		}else{
			//This is the last packet that we are going to recieve
			memcpy(&recvCode, data, 2);
			recvCode = ntohs(recvCode);
			if(recvCode == 5){
				//the client sent an error packet we can close connection
				std::cerr << "Error recieved!";
				close(clientFd);
				return;
			}else if(recvCode != 3){
				//The opcode received was wrong
				sendError(clientFd, recvFrom, recvFromLen, 4, "Illegal TFTP operation.");
				return;
			}

			if(recvAmount < 516){
				//We are done receiving packets just send the last ack
				receivedLast = true;	
			}

			memcpy(&blockNum, data+2, 2);

			//If the block number being received isn't the block number
			//we are on then we don't need to write the data to file

			if(ntohs(blockNum) == currentBlockNum){
				//We are on the proper block number we can write the data to file
				file.write(data+4, recvAmount - 4);
				//We are done with that block num so we can increment the number we are looking for
				++currentBlockNum;
				if(file.bad()){
					//If the badbit is set after writing its most likely caused by no space left
					sendError(clientFd, recvFrom, recvFromLen, 3, "Disk full or allocation exceeded.");
					return;
				}
			}
			
		}

	}while(receivedLast == false);

	//Send the last ack message to the client
	memcpy(ack+2, &blockNum, 2);
	amountSent = sendto(clientFd, ack, 4, 0, (struct sockaddr *) &recvFrom, recvFromLen);

	close(clientFd);
}