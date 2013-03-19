CC = g++
OPTIONS = -std=c++11 -Wall -Wextra

all: main.cpp sending.o senderror.o receiving.o
	$(CC) $(OPTIONS) -o tftpd main.cpp sending.o senderror.o receiving.o -pthread

sending.o: sending.cpp
	$(CC) $(OPTIONS) -c -o sending.o sending.cpp

senderror.o: senderror.cpp
	$(CC) $(OPTIONS) -c -o senderror.o senderror.cpp

receiving.o: receiving.cpp
	$(CC) $(OPTIONS) -c -o receiving.o receiving.cpp

clean:
	rm *.o
	rm tftpd
