#include "maininclude.h"
#include "sending.h"
#include "senderror.h"

int main(void){
	//Set up listening socket for new requests
	struct addrinfo hints, *res;
	int serverFD;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, "69", &hints, &res)){
		std::cerr << "Error setting up loopback\n";
		exit(1);
	}

	serverFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if(bind(serverFD, res->ai_addr, res->ai_addrlen)){
		std::cerr << "Error binding to port\n" << errno << " "<< strerror(errno);
		exit(1);
	}

	//Start waiting for incoming requests
	while(1){
		std::cerr << "WAITING\n";
		struct sockaddr_storage comingFrom;
		int readAmount;
		//Statically allocated buffer, will be able to fit any of the initial connection messages
		char buffer[512];
		short opcode = 0;
		//Max length for file names is 255 on linux
		char filename[256];
		char * afterFilename;
		//Forcing 6 because we are only worrying about octet/binary mode
		char mode[6];
		int clientFd;


		socklen_t comingFromLen = sizeof(struct sockaddr_storage);

		memset(buffer, 0, 512);
		memset(filename, 0, 255);
		memset(mode, 0, 6);

		readAmount = recvfrom(serverFD, buffer, 512, 0, (struct sockaddr *) &comingFrom, &comingFromLen);

		if(readAmount == -1){
			//There was an error reading from the socket, possibly socket problem possibly not so keep running
			std::cerr << "Error reading from socket\n" << errno << " " << strerror(errno);
			continue;
		}

		//Since the opcode is only 2 bytes long we need to copy it to the back of the integer
		memcpy((unsigned char *) &opcode, buffer, 2);
		opcode = ntohs(opcode);
		strcpy(filename, buffer+2);

		//Find the occurence of the first \0 character know that the mode is after that
		afterFilename = (char *) rawmemchr(buffer+2, '\0');

		strncpy(mode, afterFilename+1, 5);

		if(strcmp(mode, "octet") != 0){
			//Unsupported transfer type send back an error packet.
			
			continue;
		}

		//Got information from request packet
		//Now create a new socket to respond to the request

		clientFd = socket(comingFrom.ss_family, SOCK_DGRAM, 0);

		if(opcode == 1){
			//Asking for a file so we must send a file to them
			sendFile(filename);
		}else if(opcode == 2){
			//Giving us a file so we must check if we can take it
			std::cerr << "Receiving\n";
		}else{
			//Not a read or write request so send back an error
			sendError(clientFd, comingFrom, comingFromLen, "Not a valid request\n");
		}
	}


	return 0;
}
