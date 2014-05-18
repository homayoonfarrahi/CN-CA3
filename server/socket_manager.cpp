#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "socket_manager.h"

using namespace std;

Socket_manager::Socket_manager(Server* srv)
{
	this->server = srv;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cerr << "error opening socket!" << endl;
		exit(1);
	}
	this->set_accept_sockfd(sockfd);

	struct sockaddr_in srv_addr;
	bzero((char*) &srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = htons(this->server->get_port_num());

	if (::bind(sockfd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0)
	{
		cerr << "error binding socket!" << endl;
		exit(1);
	}

	listen(sockfd, 1024);

	this->accept_thread = new thread(Socket_manager::accept_connection, this);
}


Socket_manager::~Socket_manager()
{
	delete this->accept_thread;
	for (int i=0 ; i<this->receiver_threads.size() ; ++i)
		delete this->receiver_threads[i];
}


void Socket_manager::accept_connection(Socket_manager* socket_manager)
{
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	while(1)
	{
		int cli_sock = accept(socket_manager->get_accept_sockfd(), (struct sockaddr*) &cli_addr, &clilen);
		//cout<<cli_sock<<endl ;
		if (cli_sock < 0)
		{
			cerr << "error accepting!" << endl;
			exit(1);
		}

		Ethernet_card* eth = NULL;
		thread* receiver_thread = new thread(Socket_manager::receive, socket_manager, cli_sock, eth);
		socket_manager->receiver_threads.push_back(receiver_thread);
	}
}


void Socket_manager::send(char* buffer, int cli_fd)
{
	while ( write ( cli_fd, buffer, PACKET_SIZE) < 0)
			continue ;
}


void Socket_manager::receive(Socket_manager* socket_manager, int cli_sock, Ethernet_card* ethernet_card)
{
	char buffer[PACKET_SIZE];

	while(1)
	{
		bzero(buffer, sizeof(buffer));

		cout << "waiting on a packet..." << endl;
		while( read(cli_sock, buffer, PACKET_SIZE) < 0);
		cout << "decoding packet..." << endl;

		// has requested disconnect
		if (buffer[0] == 0)
		{
			ethernet_card->set_busy(false);
			ethernet_card->set_cli_sock(-1);
			string ethernet_name = ethernet_card->get_name();
			vector<vector<string> >* table = socket_manager->get_server()->get_table();
			for (int i=0 ; i<table->size() ; ++i)
				if ((*table)[i][0] == ethernet_name)
				{
					table->erase(table->begin() + i);
					break;
				}
			break;
		}

		// see what eth card she wants to connect to
		if (buffer[0] == 1)
		{
			stringstream cnct_ss;
			cnct_ss << buffer;
			char pack_id;
			cnct_ss >> pack_id;
			string eth_name;
			cnct_ss >> eth_name;
			cout << "connecting to card: " << eth_name << endl;

			Ethernet_card* eth = socket_manager->server->get_ethernet_card(eth_name);
			eth->set_busy(true);
			eth->set_cli_sock(cli_sock);
			ethernet_card = eth;

			break;
		}


		stringstream ss;
		ss << buffer;

		// Extract source ip
		string source_ip;
		ss >> source_ip;
		cout << source_ip << endl;

		// Extract source mac
		string source_mac;
		ss >> source_mac;
		cout << source_mac << endl;

		// Extract destination ip
		string destination_ip;
		ss >> destination_ip;
		cout << destination_ip << endl;

		// Extract sequence number
		string sequence_number;
		ss >> sequence_number;
		cout << sequence_number << endl;

		// Extract ping(0) or reply(1)
		string ping_reply;
		ss >> ping_reply;
		cout << ping_reply << endl;

		// Extract vlan if default(u) set correct vlan
		string vlan;
		ss >> vlan;
		if (vlan == "u")
		{
			memset(buffer, 0, sizeof(buffer));

			int index = 0;

			// Append source ip address
			for (int i=0 ; i<source_ip.length() ; ++i)
			{
				buffer[index] = source_ip[i];
				index++;
			}
			buffer[index] = ' ';
			index++;

			// Append source mac address
			for (int i=0 ; i<source_mac.length() ; ++i)
			{
				buffer[index] = source_mac[i];
				index++;
			}
			buffer[index] = ' ';
			index++;

			// Append destination ip address
			for (int i=0 ; i<destination_ip.length() ; ++i)
			{
				buffer[index] = destination_ip[i];
				index++;
			}
			buffer[index] = ' ';
			index++;

			// Append sequence number
			buffer[index] = sequence_number.c_str()[0];
			index++;
			buffer[index] = ' ';
			index++;

			// Append ping(0) or reply(1)
			buffer[index] = ping_reply.c_str()[0];
			index++;
			buffer[index] = ' ';
			index++;

			// Append new vlan(u)
			buffer[index] = ethernet_card->get_vlan() + 48;
			index++;
			buffer[index] = ' ';
			index++;
			// vlan = new vlan
		}

		// Add source to table if it doesn't exist
		bool found = false;
		vector<vector<string> >* table = socket_manager->get_server()->get_table();
		for (int i=0 ; i<table->size() ; ++i)
			if ((*table)[i][2] == source_ip)
			{
				found = true;
				break;
			}

		if (!found)
		{
			vector<string> entry;
			entry.push_back(ethernet_card->get_name());
			entry.push_back(source_mac);
			entry.push_back(source_ip);
			entry.push_back(vlan);
			table->push_back(entry);
		}


		// Look to find destination
		found = false;
		string output_ethernet_name;
		for (int i=0 ; i<table->size() ; ++i)
			if ((*table)[i][2] == destination_ip)
			{
				found = true;
				output_ethernet_name = (*table)[i][0];
				break;
			}

		Ethernet_card* destination_ethernet_card = socket_manager->get_server()->get_ethernet_card(output_ethernet_name);

		if (found)
		{
			socket_manager->send(buffer, destination_ethernet_card->get_cli_sock());
		}
		else
		{
			vector<Ethernet_card*> all_cards = socket_manager->get_server()->get_ethernet_cards();
			for (int i=0 ; i<all_cards.size() ; ++i)
			{
				if (all_cards[i]->get_busy())
				{
					output_ethernet_name = all_cards[i]->get_name();
					destination_ethernet_card = socket_manager->get_server()->get_ethernet_card(output_ethernet_name);
					socket_manager->send(buffer, destination_ethernet_card->get_cli_sock());
				}
			}
		}



		// socket_manager->get_server()->get_packet_manager()->decode(buffer,cli_sock);

		// stringstream ss;
		// ss << buffer;
		// string token;
		// ss >> token;
		// if (token == "1001")
		// {
		// 	break;
		// }

		// cout<<buffer<<endl ;
		// while( write(cli_sock, buffer, PACKET_SIZE) < 0)
		// 	continue ;

	}
}

void Socket_manager::connect_to_switch(Ethernet_card* my_ethernet_card, int port_num)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cerr << "error opening socket!" << endl;
		exit(0);
	}

	struct hostent *server;
	server = gethostbyname("localhost");
	struct sockaddr_in srv_addr;
	bzero((char*) &srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("localhost");
	bcopy((char*)server->h_addr, (char*)&srv_addr.sin_addr.s_addr, server->h_length);
	srv_addr.sin_port = htons(port_num);

	if (connect(sockfd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0)
	{
		cerr << "error connecting!" << endl;
		exit(0);
	}

	my_ethernet_card->set_cli_sock(sockfd);
	my_ethernet_card->set_busy(true);

	receiver = new thread(Socket_manager::receive, this, sockfd, my_ethernet_card);
}












