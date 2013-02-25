CC = g++
OPTIONS = -Wall -Wextra

all: main.cpp
	$(CC) $(OPTIONS) -o tftpd main.cpp
