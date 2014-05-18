all: server

server: main.cpp server.o socket_manager.o ethernet_card.o
	g++ -std=c++0x main.cpp server.o socket_manager.o ethernet_card.o -lpthread -o server

server.o: server.cpp server.h
	g++ -std=c++0x -c server.cpp

socket_manager.o: socket_manager.cpp socket_manager.h
	g++ -std=c++0x -c socket_manager.cpp

ethernet_card.o: ethernet_card.cpp ethernet_card.h
	g++ -std=c++0x -c ethernet_card.cpp
