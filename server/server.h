#ifndef SERVER_H
#define SERVER_H

class Socket_manager;

#include <string>
#include <vector>
#include "socket_manager.h"
#include "ethernet_card.h"

using namespace std ;

class Server
{
public:
	Server();
	virtual ~Server();

	void run() ;

	int get_port_num() { return port_num; }
	vector<Ethernet_card*>& get_ethernet_cards() { return ethernet_cards; }
	vector<vector<string> >* get_table() { return &table; }

	Ethernet_card* get_ethernet_card(string n);
	
	void test() ;

private:
	int port_num;
	Socket_manager* socket_manager;
	vector<Ethernet_card*> ethernet_cards;
	vector<vector<string> > table;
};


#endif
