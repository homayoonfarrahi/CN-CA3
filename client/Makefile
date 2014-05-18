all: client

client: main.cpp client.o socket_manager.o
	g++ -std=c++0x main.cpp client.o socket_manager.o -lpthread -o client
	
client.o: client.cpp client.h
	g++ -std=c++0x -c client.cpp
	
socket_manager.o: socket_manager.cpp socket_manager.h
	g++ -std=c++0x -c socket_manager.cpp
