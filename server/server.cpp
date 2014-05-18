#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "server.h"

using namespace std;

Server::Server()
{
	
}

Server::~Server()
{
	delete this->socket_manager;
	for (int i=0 ; i<ethernet_cards.size() ; ++i)
	{
		delete ethernet_cards[i];
	}
}

void Server::run()
{
	while(1)
	{
		cout << ">>> ";
		stringstream ss;
		string line;
		string command;
		getline(cin, line);
		ss << line;

		ss >> command;

		if (command == "ListenPort" || command == "listenport")
		{
			int  port;
			ss >> port;
			port_num = port;
			this->socket_manager = new Socket_manager(this);
		}

		else if (command == "Eth" || command == "eth")
		{
			string eth_name;
			ss >> eth_name;
			this->ethernet_cards.push_back(new Ethernet_card(eth_name));
		}

		else if (command == "NoEth" || command == "noeth")
		{
			string eth_name;
			ss >> eth_name;
			for (int i=0 ; i<ethernet_cards.size() ; ++i)
			{
				if (ethernet_cards[i]->get_name() == eth_name)
					ethernet_cards.erase(ethernet_cards.begin() + i);
			}
		}

		else if (command == "IP" || command == "ip")
		{
			string eth_name;
			ss >> eth_name;
			string ip_addr;
			ss >> ip_addr;
			Ethernet_card* eth = get_ethernet_card(eth_name);
			eth->set_ip_addr(ip_addr);
		}

		else if (command == "NoIP" || command == "noip")
		{
			string eth_name;
			ss >> eth_name;
			Ethernet_card* eth = get_ethernet_card(eth_name);
			eth->set_ip_addr("");
		}

		else if (command == "Vlan" || command == "vlan")
		{
			string eth_name;
			int vlan;
			ss >> eth_name;
			ss >> vlan;
			for (int i=0 ; i<ethernet_cards.size() ; ++i)
			{
				if (ethernet_cards[i]->get_name() == eth_name)
					ethernet_cards[i]->set_vlan(vlan);
			}
		}

		else if (command == "Show" || command == "show")
		{
			for (int i=0 ; i<table.size() ; ++i)
			{
				cout << "eth: " << table[i][0] << " mac: " << table[i][1] << " ip: " << table[i][2] << " vlan: " << table[i][3] << endl;
			}
		}

		else if (command == "Connect" || command == "connect")
		{
			string my_card;
			ss >> my_card;
			string her_card;
			ss >> her_card;
			int her_port_num;
			ss >> her_port_num;

			Ethernet_card* my_ethernet_card = get_ethernet_card(my_card);
			this->socket_manager->connect_to_switch(my_ethernet_card, her_port_num);
		}

		else if (command == "ChangeCost" || command == "changecost")
		{
			string eth_name;
			int new_cost;
			ss >> eth_name;
			ss >> new_cost;
			Ethernet_card* eth = get_ethernet_card(eth_name);
			eth->set_cost(new_cost);
		}

		else if (command == "Disconnect" || command == "disconnect")
		{
			string my_card;
			ss >> my_card;

			Ethernet_card* my_ethernet_card = get_ethernet_card(my_card);
			char buffer[PACKET_SIZE];
			memset(buffer, 0, sizeof(buffer));
			this->socket_manager->send(buffer, my_ethernet_card->get_cli_sock());

			my_ethernet_card->set_busy(false);
			my_ethernet_card->set_cli_sock(-1);
		}
	}
}

Ethernet_card* Server::get_ethernet_card(string n)
{
	for (int i=0 ; i<ethernet_cards.size() ; ++i)
		if (ethernet_cards[i]->get_name() == n)
			return ethernet_cards[i];
	return NULL;
}








