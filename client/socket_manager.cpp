#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include "socket_manager.h"

using namespace std;

Socket_manager::Socket_manager(Client* cli, int port_num, string eth_name)
{
	this->client = cli;

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

	this->server_fd = sockfd;

	// send a packet to tell which eth_card to connect to
	char buffer[PACKET_SIZE];
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 1;
	buffer[1] = ' ';
	int index = 2;

	// Append eth_name
	for (int i=0 ; i<eth_name.length() ; ++i)
	{
		buffer[index] = eth_name[i];
		index++;
	}
	send(buffer);

	receiver = new thread(Socket_manager::receive, this);
}


Socket_manager::~Socket_manager()
{
	delete receiver;
}


void Socket_manager::send(char* buffer)
{
	while ( write ( this->server_fd, buffer, PACKET_SIZE) < 0)
		continue ;

}


void Socket_manager::receive(Socket_manager* socket_manager)
{
	char buffer[PACKET_SIZE];

	while(1)
	{
		while( read(socket_manager->server_fd, buffer, PACKET_SIZE) < 0)
			continue;
		// cout << buffer << endl;

		stringstream ss;
		ss << buffer;

		// Extract source ip
		string source_ip;
		ss >> source_ip;

		// Extract source mac
		string source_mac;
		ss >> source_mac;

		// Extract destination ip
		string destination_ip;
		ss >> destination_ip;
		// Drop packet if not for you
		if (socket_manager->get_client()->get_ip_address() != destination_ip)
			continue;

		// Extract sequence number
		string sequence_number;
		ss >> sequence_number;

		// Extract ping(0) or reply(1)
		string ping_reply;
		ss >> ping_reply;

		if (ping_reply == "0")
		{
			memset(buffer, 0, sizeof(buffer));
			int index = 0;

			// Append source ip address
			for (int i=0 ; i<destination_ip.length() ; ++i)
			{
				buffer[index] = destination_ip[i];
				index++;
			}
			buffer[index] = ' ';
			index++;

			// Append source mac address
			string mac_address = socket_manager->get_client()->get_mac_address();
			for (int i=0 ; i<mac_address.length() ; ++i)
			{
				buffer[index] = mac_address[i];
				index++;
			}
			buffer[index] = ' ';
			index++;

			// Append destination ip address
			for (int i=0 ; i<source_ip.length() ; ++i)
			{
				buffer[index] = source_ip[i];
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
			char ping_reply = 1 + 48;
			buffer[index] = ping_reply;
			index++;
			buffer[index] = ' ';
			index++;

			// Append default vlan(u)
			char vlan_char = 'u';
			buffer[index] = vlan_char;
			index++;
			buffer[index] = ' ';
			index++;


			// Send buffer
			socket_manager->send(buffer);

		}
		else if (ping_reply == "1")
		{
			cout << "8 bytes from " << source_ip << ": icmp_seq=" << sequence_number << " ttl=64" << endl;
		}

		// Extract vlan if default(u) set correct vlan
		// string vlan;
		// ss >> vlan;
		// if (vlan == "u")
		// {
		// 	// bzero buffer
		// 	// fill buffer all over again
		// 	// vlan = new vlan
		// }

	}
}











