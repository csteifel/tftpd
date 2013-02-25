CC = g++
OPTIONS = -std=c++11 -Wall -Wextra

all: main.cpp sending.o
	$(CC) $(OPTIONS) -o tftpd main.cpp sending.o

sending.o: sending.cpp
	$(CC) $(OPTIONS) -c -o sending.o sending.cpp